#!/usr/bin/env node

let path = require('path');
let yazl = require('yazl');
let fs = require('fs');
let { target_cpu, exec, project_name, project_version } = require('./common');

exec('node scripts/build.js Release');

let extension = process.platform === 'win32'? '.exe' : '';
let exepath = `target/Release/${project_name}${extension}`;

let zip = new yazl.ZipFile;
zip.addFile('cpp_modules/libyue/LICENSE', 'LICENSE');
zip.addFile(exepath, path.basename(exepath), { mode: parseInt("0100755", 8) });
zip.outputStream.pipe(fs.createWriteStream(`${project_name}-v${project_version}-${target_cpu}.zip`));
zip.end();