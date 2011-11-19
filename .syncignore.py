#coding=utf-8

import re

ignore_names = (
    re.compile('.*\\.suo$', re.IGNORECASE),
    re.compile('.*\\.user$', re.IGNORECASE),
    re.compile('.*\\.opensdf$', re.IGNORECASE),
    re.compile('.*\\.sdf$', re.IGNORECASE),
)

ignore_paths = (
    'wks/MSVC/Debug',
    'wks/MSVC/ipch',
    'wks/MSVC/MSVC/Debug',
    'tmp',
)
