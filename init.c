
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>



struct MemoryStruct {
    char *memory;
    size_t size;
};

size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, struct MemoryStruct *userp) {
    size_t realsize = size * nmemb;
    userp->memory = realloc(userp->memory, userp->size + realsize + 1);
    if(userp->memory == NULL) {
        // Out of memory!
        printf("Not enough memory (realloc returned NULL)\n");
        return 0;
    }
    
    memcpy(&(userp->memory[userp->size]), contents, realsize);
    userp->size += realsize;
    userp->memory[userp->size] = 0; // Null-terminate
    return realsize;
}


void print_with_delay(const char *str, int delay) {
    while (*str) {
        putchar(*str++);
        fflush(stdout);
        usleep(delay * 222); // Delay in milliseconds
    }
    putchar('\n');
}

void parse_and_print_response(const char *response) {
    cJSON *json = cJSON_Parse(response); // Parse the JSON response
    if (json == NULL) {
        printf("Error parsing JSON response.\n");
        return;
    }

    // Extract the "response" field from the JSON object
    cJSON *message = cJSON_GetObjectItemCaseSensitive(json, "response");
    if (cJSON_IsString(message) && (message->valuestring != NULL)) {
        char full_message[2048]; // Adjust size as necessary
        snprintf(full_message, sizeof(full_message), "Jane: %s", message->valuestring);
        print_with_delay(full_message, 44); 
    } else {
        printf("No valid response found in JSON.\n");
    }

    cJSON_Delete(json); // Clean up JSON object
}

void escape_json_string(const char *input, char *output, size_t output_size) {
    size_t j = 0;
    for (size_t i = 0; i < strlen(input) && j < output_size - 1; ++i) {
        switch (input[i]) {
            case '\"': // Escape double quotes
                if (j + 2 < output_size) {
                    output[j++] = '\\';
                    output[j++] = '\"';
                }
                break;
            case '\\': // Escape backslashes
                if (j + 2 < output_size) {
                    output[j++] = '\\';
                    output[j++] = '\\';
                }
                break;
            case '\n': // Escape newlines
                if (j + 2 < output_size) {
                    output[j++] = '\\';
                    output[j++] = 'n';
                }
                break;
            default:
                output[j++] = input[i];
                break;
        }
    }
    output[j] = '\0'; // Null-terminate the escaped string
}

void truncate_response(const char *response, char *truncated_response, size_t max_length) {
    size_t length = strlen(response);
    if (length > max_length) {
        strncpy(truncated_response, response, max_length - 3); // Reserve space for "..."
        truncated_response[max_length - 3] = '\0'; // Null-terminate
        strcat(truncated_response, "..."); // Append ellipsis
    } else {
        strcpy(truncated_response, response);
    }
}

void send_to_transformer(const char *prompt) {
    static char conversation_history[1024] = "";  
    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;
    struct MemoryStruct response = {NULL, 0}; 

    // Check for empty input
    if (strlen(prompt) == 0) {
        printf("Received empty input, not sending to server.\n");
        return;
    }

    char truncated_prompt[128];
    truncate_response(prompt, truncated_prompt, sizeof(truncated_prompt));

    // Append the user's prompt to the conversation history
    snprintf(conversation_history + strlen(conversation_history),
             sizeof(conversation_history) - strlen(conversation_history) - 1,
             "User: %s\n", truncated_prompt);


    // Initialize CURL
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        char json_data[4096];
        char escaped_conversation_history[2048];

        // Escape the conversation history
        escape_json_string(conversation_history, escaped_conversation_history, sizeof(escaped_conversation_history));

        // Construct the JSON data
        snprintf(json_data, sizeof(json_data), "{\"prompt\":\"%s\"}", escaped_conversation_history);

        curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:7272/groq");
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            
            char truncated_response[128];
            truncate_response(response.memory, truncated_response, sizeof(truncated_response));

            // Update conversation history with the bot response
            strcat(conversation_history, "Jane: ");
            strncat(conversation_history, truncated_response, sizeof(truncated_response)); // Use truncated_response here
            strcat(conversation_history, "\n");

            parse_and_print_response(response.memory); 
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        free(response.memory); 
    }
    curl_global_cleanup();
}



int main() {
    const char *jane_introduction = "Your name is Jane (like the AI from Ender's Game and Speaker for The Dead) and you are eager to begin discussion of whatever topic the user inputs.";
    send_to_transformer(jane_introduction); 

    char input[256]; // Buffer for user input

    while (1) {
        printf("INPUT: ");
        if (fgets(input, sizeof(input), stdin) != NULL) {
            input[strcspn(input, "\n")] = '\0';  
            send_to_transformer(input); // Send input to the /groq endpoint
        }
    }

    return 0;
}