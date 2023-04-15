# coding: utf-8

import sys
import importlib

PACKAGE_NAME = 'ml.learning.'


def run():
    """
    sys.argv
    0: file name: main_l.py
    1: learning module name
    2: iterate num
    """
    if len(sys.argv) < 2:
        print('need module name')
        return
    elif len(sys.argv) < 3:
        iterate_num = 1
    else:
        iterate_num = int(sys.argv[2])
    module_name = sys.argv[1]
    learning = importlib.import_module(PACKAGE_NAME + module_name)
    learning_obj = learning.Learning()
    for i in range(iterate_num):
        print('learning: {}'.format(i))
        loss, accuracy = learning_obj.learn()
        print('loss: {}, accuracy: {}'.format(loss, accuracy))
    print('fin')


def run2():
    """
    convert for final model
    """
    if len(sys.argv) < 2:
        print('need module name')
        return
    module_name = sys.argv[1]
    learning = importlib.import_module(PACKAGE_NAME + module_name)
    learning_obj = learning.Learning()
    learning_obj.remove_optimizer_data()
    print('fin')


if __name__ == '__main__':
    run()
    run2()
