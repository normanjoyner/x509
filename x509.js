// import dependencies
var ursa = require("ursa");
var x509 = require([__dirname, "build", "Release", "x509cert"].join("/"));

exports.getObject = function(){

    var object = {
        cert: new x509.wrapper(),

        getPublicKey: function(){
            return this.cert.getPublicKey();
        }
    }

    return object;
}
