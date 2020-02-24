const { adapter } = require('bindings')('adapter');

adapter(function(msg){
	console.log(msg);
});
