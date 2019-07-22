
TEMPLATE = subdirs

SUBDIRS += \
    nut \
    test-nut

test-nut.depends = nut
