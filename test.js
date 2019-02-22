const fs = require("fs");
const {fallocateSync} = require(".");

const path = "./test.dat";
const fd = fs.openSync(path, "w", 0o666);
console.log("pre:", fs.statSync(path).size);
try {
	fallocateSync(fd, 1234);
} catch (e) {
	console.log(e.errno);
	throw e;
}
console.log("post:", fs.statSync(path).size);

fs.closeSync(fd);
fs.unlinkSync(path);
