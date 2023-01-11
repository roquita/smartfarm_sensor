# ESP32 LIGH WEBPANEL
Basic web gui for esp32 developed using webpack and node js.

## How to understand this repository?
* This project uses an environment based on webpack and node js to bundle all files.
* The output files are stored inside "build" directory.
* There are development and production command lines to generate output files.
* Development command line create a local server to serve output files, and tracks changes in real time.
* Producction command line generate minified and compressed(gzip) output files ready to deploy. User
can disable compression if needed.
* By default, there are two views: login and main.

## How to add it to my project?
* Save output files into esp32.
* On esp32 side must be paths to serve every output file.
```c
Ex: {esp32-local-ip}:{esp32-port}/main.css  to serve  "main.css" file.
Ex: {esp32-local-ip}:{esp32-port}/login.css  to serve  "login.css" file.
```

## How to add features or modify it?
* Clone the repository.
* Install node js dependencies.
* Implement functionality needed in javascript files.
* run "npm run start" for development or "npm run build" for production.

## Note
* This repository only offer frontend to user.
* Use postman collection to mockup a testing server.
* Node JS version 16.13.0
* Manually compress favicon.ico fileto gzip.
