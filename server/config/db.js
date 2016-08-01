
 var mongoose = require('mongoose');

 var config = {
   url: process.env.MONGO_URI || 'mongodb://localhost/homeauto'
 }; // The default port of MongoDB is 27017

 module.exports = function() {
   mongoose.connect(config.url);
 };
