#!/usr/bin/python
# -*- coding: utf-8 -*-

import os,sys

qtsdk_win32_root = 'D:/data/app/win/develop/QtSDK'
cd_cmd = 'cd "' + os.path.abspath(os.path.join(sys.argv[0], '../build')) + '"'
pro_path = '../pro/nut.pro'

def _setup_path():
	if sys.platform == 'win32':
		path = os.environ['path']
		path = path + ';' + os.path.abspath(os.path.join(qtsdk_win32_root, '5.3/mingw482_32/bin'))
		path = path + ';' + os.path.abspath(os.path.join(qtsdk_win32_root, 'Tools/mingw482_32/bin'))
		print(path)
		os.environ['path'] = path

def build_win32(debug):
	# qmake
	cmd = cd_cmd + ' && qmake ' + pro_path + ' -r -spec win32-g++'
	if debug:
		cmd = cmd + ' CONFIG+=debug'
	print(cmd)
	if 0 != os.system(cmd):
		raise Exception('qmake failed')

	# make
	cmd = cd_cmd + ' && mingw32-make'
	print(cmd)
	if 0 != os.system(cmd):
		raise Exception('make failed')
		
def clean_win():
	# make clean
	cmd = cd_cmd + ' && mingw32-make clean'
	print(cmd)
	if 0 != os.system(cmd):
		raise Exception('make failed')

def build_mac64(debug):
	# qmake
	cmd = cd_cmd + ' ; qmake ' + pro_path + ' -r -spec macx-clang CONFIG+=x86_64'
	if debug:
		cmd = cmd + ' CONFIG+=debug'
	print(cmd)
	if 0 != os.system(cmd):
		raise Exception('qmake failed')

	# make
	cmd = cd_cmd + ' ; make'
	print(cmd)
	if 0 != os.system(cmd):
		raise Exception('make failed')
		
def clean_mac():
	# make clean
	cmd = cd_cmd + ' ; make clean'
	print(cmd)
	if 0 != os.system(cmd):
		raise Exception('make failed')

def build_linux64():
	# qmake
	cmd = cd_cmd + ' ; qmake ' + pro_path + ' -r -spec linux64-g++'
	if debug:
		cmd = cmd + ' CONFIG+=debug'
	print(cmd)
	if 0 != os.system(cmd):
		raise Exception('qmake failed')

	# make
	cmd = cd_cmd + ' ; make'
	print(cmd)
	if 0 != os.system(cmd):
		raise Exception('make failed')

def clean_linux():
	# make clean
	cmd = cd_cmd + ' ; make clean'
	print(cmd)
	if 0 != os.system(cmd):
		raise Exception('make failed')

def main():
	# 处理参数
	clean = False
	build = True
	debug = True
	for i in range(1, len(sys.argv)):
		if sys.argv[i] == 'help':
			print('build [debug|release] [help|build|clean|rebuild]')
			return
		elif sys.argv[i] == 'build':
			clean = False
			build = True
		elif sys.argv[i] == 'clean':
			clean = True
			build = False
		elif sys.argv[i] == 'rebuild':
			clean = True
			build = True
		elif sys.argv[i] == 'debug':
			debug = True
		elif sys.argv[i] == 'release':
			debug = False
		else:
			raise Exception('Unexpected argument: ' + sys.argv[i])

	_setup_path()
			
	if clean:
		if sys.platform == 'win32':
			clean_win()
		elif sys.platform == 'darwin':
			clean_mac()
		elif sys.platform == 'linux2':
			clean_linux()
		else:
			raise Exception('Unknown platform: ' + sys.platform)

	if build:
		if sys.platform == 'win32':
			build_win32(debug)
		elif sys.platform == 'darwin':
			build_mac64(debug)
		elif sys.platform == 'linux2':
			build_linux64(debug)
		else:
			raise Exception('Unknown platform: ' + sys.platform)

if __name__ == '__main__':
	main()
