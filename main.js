// import dependencies
var x509 = require([__dirname, "x509"].join("/"));

exports = module.exports = function(){
    var object = x509.getObject();
    object.version = "0.0.1";
    return object;
}
