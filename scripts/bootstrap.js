#!/usr/bin/env node

let fs = require('fs');
let path = require('path')
let { target_cpu, cmake, mkdir, spawn } = require('./common')

mkdir('target');
fs.symlinkSync(path.resolve('./resources'), path.resolve('./target/resources'), 'dir');

if (process.platform === 'win32') {
    let generator = 'Visual Studio 15';
    generator += target_cpu === 'x64'? ' Win64' : '';
    spawn(cmake, ['..', '-G', generator], {cwd: 'target'})
}

if (process.platform === 'darwin') {
    spawn(cmake, ['.', '-G', 'Xcode'], {cwd: 'target'});
}

if (process.platform === 'linux') {
    mkdir('target/Release');
    spawn(cmake, ['-D', `CMAKE_BUILD_TYPE=Release`, '../..'], {cwd: 'target/Release'});
    mkdir('target/Debug');
    spawn(cmake, ['-D', `CMAKE_BUILD_TYPE=Debug`, '../..'], {cwd: 'target/Debug'});
}