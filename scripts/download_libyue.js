#!/usr/bin/env node

let fs = require('fs');
let path = require('path');
let extract = require('extract-zip');
let { target_cpu, download, mkdir } = require('./common');

if (process.argv.length < 3) {
    console.error('Usage: download_libyue version platform arch');
    process.exit(1);
}

let [,, version, platform, arch] = process.argv;

if (!platform) {
    platform = {
        win32: 'win',
        linux: 'linux',
        darwin: 'mac',
    }[process.platform];
}

arch = arch || target_cpu;

if (!fs.existsSync('cpp_modules')) {
    fs.mkdirSync('cpp_modules');
}

let libyueDir = path.resolve('cpp_modules/libyue');
let libyueChecksum = path.join(libyueDir, '.version');
let checksum = `${version}|${platform}|${arch}`;

if (fs.existsSync(libyueChecksum) && fs.readFileSync(libyueChecksum).toString() == checksum) {
    process.exit(0);
}

let mirror = 'https://github.com/yue/yue/releases/download';
let zipname = `libyue_${version}_${platform}_${arch}.zip`;
let url = `${mirror}/${version}/${zipname}`;

download(url, (response) => {
    mkdir(libyueDir);

    let zippath = path.join(libyueDir, zipname);

    response.on('end', () => {
        extract(zippath, {dir: libyueDir}, (error) => {
            fs.unlinkSync(zippath);
            fs.writeFileSync(libyueChecksum, checksum);
        });
    });
    
    response.pipe(fs.createWriteStream(zippath));
});