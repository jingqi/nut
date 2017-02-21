
TEMPLATE = subdirs

SUBDIRS += \
    nut \
    test_nut

test_nut.depends = nut
