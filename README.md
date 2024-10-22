# Console Chatbot | Readme.md

*v0.0.1*

---

A simple console app using llama3-8b-8192 via Express server & Groq.com library.

## Install & Config

**Open a terminal and clone this repo:**
- *git clone URL*

**Nav into app directory:**
- *cd folder-name*

**Set up the node server:**
- *cd /res/transformer*
- Configure .env with "PORT" and ["GROQ_API" key](https://console.groq.com/keys)
- use "*npm install*" to install modules and depenencies
- use "*npm run start*" start the node server

**Compile the executable:**

In a new terminal, nav to the app's root directory: "/folder-name"

Compile the app file with: 
- *gcc init.c -o app -lcurl -lcjson*

Make the compiled app file executeable:
- run "*chmod +x app*"

**Initialize the chat**
- run "*./app*" to begin speaking with your new chatbot directly in the terminal

## Overview:

**init.c & application binary:**
- The main file to initialize the app, handle input/output, and manage conversation context
- Forwards input from terminal to transformer via node.js server and prints response
- You may edit init.c to change the prompt or print speed to your liking
- If compilation fails initially, check init.c to ensure you've installed the required dependencies 
  - or run "*sudo apt-get install libcurl4-openssl-dev libjson-c-dev*" to install them (on debian/ubuntu)

**README.md:**
- (*You are here*)

**/res:**

- /res/transformer
  - Simple Node/Express server for handling the Groq library integration
  - Edit server file to adjust token use and other Groq library options
  - See package.json for dependency and version details

*NEXT: v0.0.2, Improved context/memory and character/knowledgebank*

---

## Acknowledgements

This project is licensed under the MIT License. Please see the LICENSE file in the repository for details on third-party libraries and their respective licenses.

Built with:

- [Node.js](https://nodejs.org/en)
  - [body-parser](https://www.npmjs.com/package/body-parser)
  - [dotenv](https://www.npmjs.com/package/dotenv)
  - [express](https://www.npmjs.com/package/express)
  - [groq-sdk](https://www.npmjs.com/package/groq-sdk)
- [C Programming Language](https://www.iso.org/standard/74528.html) 
  - [libcurl](https://curl.se/libcurl/)
  - [cJSON](https://github.com/DaveGamble/cJSON)
- [Groq Console](https://console.groq.com/)
- [Groq Docs](https://console.groq.com/docs/quickstart)
- [Llama-3-8B](https://huggingface.co/meta-llama/Meta-Llama-3-8B-Instruct)

