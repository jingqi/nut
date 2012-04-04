#coding=utf-8

import re

ignore_names = (
    re.compile('.*\\.suo$', re.IGNORECASE),
    re.compile('.*\\.user$', re.IGNORECASE),
    re.compile('.*\\.opensdf$', re.IGNORECASE),
    re.compile('.*\\.sdf$', re.IGNORECASE),
    re.compile('.*\\.pyc$', re.IGNORECASE),
)

ignore_paths = (
    'wks/MSVC/Debug',
    'wks/MSVC/Release',
    'wks/MSVC/ipch',
    'wks/MSVC/MSVC/Debug',
    'wks/MSVC/MSVC/Release',
    'tmp',
	'wks/makefile/makefile',
	'wks/makefile/output',
)
