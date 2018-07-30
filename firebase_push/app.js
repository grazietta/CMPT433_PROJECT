const admin = require('firebase-admin');
const {
	exec
} = require('child_process');

const serviceAccount = require('./serviceAccount');

var dgram = require('dgram')

var PORT = 12345
var HOST = '192.168.7.1'

admin.initializeApp({
	credential: admin.credential.cert(serviceAccount),
	databaseURL: 'https://smiledetection-d7b07.firebaseio.com/'
});

var ref = admin.database().ref('modes')
var server = dgram.createSocket('udp4')

server.on('listening', function () {
	var address = server.address()
	console.log('UDP server listening ' + address.address + ':' + address.port)
})

server.on('message', function (message, remote) {
	console.log(message.toString())

	ref.set({
		"mode": message.toString()
	})
})

server.bind(PORT, HOST)