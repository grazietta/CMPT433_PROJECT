/*
Server that renders an html file containing a chart that recieves real-time data from firebase and graphs it
*/

var express = require('express');
var app = express();
var path = require('path');

app.get('/', function(req, res) {
    res.sendFile(path.join(__dirname + '/index.html'));
});

app.listen(8080);