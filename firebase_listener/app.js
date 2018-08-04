/*
NodeJs server that listens to incoming changes from firebase
*/

const Firebase = require("firebase");
const express = require("express");
const admin = require('firebase-admin');
const {
    exec
} = require('child_process');

const serviceAccount = require('./serviceAccount');

admin.initializeApp({
    credential: admin.credential.cert(serviceAccount),
    databaseURL: 'https://smiledetection-d7b07.firebaseio.com/'
});

// Create HTTP Server
const app = express();
const server = require("http").createServer(app);

// Indicate port 3000 as host
const port = process.env.PORT || 3000;

// Make the server listens on port 3000
server.listen(port, function () {
    console.log("Server listening on port %d", port);
});

const ref = admin.database().ref("modes");
ref.on('value', function (snapshot) {

    let mode = snapshot.val().mode
    console.log(mode)

    // exec(`./arduino-serial -b 9600 -p /dev/tty.usbserial -s ${mode}`, (err, stdout, stderr) => {
    //     if (err) {
    //         // node couldn't execute the command
    //         return;
    //     }
    //})
})