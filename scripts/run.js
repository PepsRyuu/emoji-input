#!/usr/bin/env node

let { spawn, build_type, project_name } = require('./common');

let extension = process.platform === 'win32'? '.exe' : '';
spawn(`target/${build_type}/${project_name}${extension}`, []);
