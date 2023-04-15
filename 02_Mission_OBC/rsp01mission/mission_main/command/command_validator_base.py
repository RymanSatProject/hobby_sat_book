# -*- coding: utf-8 -*-
from mission_main.command import get_module_logger


class CommandValidatorBase:
    """
    コマンドのリクエストの検証クラスのBase
    C&DHからのコマンドを検証する処理はこのクラスを継承して作成してください。
    """

    def __init__(self):
        self.logger = get_module_logger(__name__)

    def _is_not_none(self, property_name, property_value):
        error_message = "{0} can't be None. value: ({1}).".format(property_name, property_value)

        if property_value is None:
            self.logger.warning(error_message)
            return False

        return True

    def _is_string(self, property_name, property_value):
        error_message = "{0} must be string. value: ({1}).".format(property_name, property_value)

        if not self._is_not_none(property_name, property_value):
            return False

        if type(property_value) is not str:
            self.logger.warning(error_message)
            return False

        return True

    def _is_integer(self, property_name, property_value):
        error_message = "{0} must be integer. value: ({1}).".format(property_name, property_value)

        if not self._is_not_none(property_name, property_value):
            return False

        if type(property_value) is not int:
            self.logger.warning(error_message)
            return False

        return True

    def _is_presence(self, property_name, property_value):
        error_message = "{0} can't be blank. value: ({1}).".format(property_name, property_value)

        if not self._is_not_none(property_name, property_value):
            return False

        if type(property_value) is str and not property_value:
            self.logger.warning(error_message)
            return False

        return True

    def _is_include_in(self, start, end, property_name, property_value):
        error_message = "{0} must to be set value from {1} to {2}. value: ({3}).".format(property_name, start, end, property_value)

        if not self._is_not_none(property_name, property_value):
            return False

        if not self._is_integer(property_name, property_value):
            return False

        if property_value < start or end < property_value:
            self.logger.warning(error_message)
            return False

        return True

    def _is_greater_than(self, minimum_number, property_name, property_value):
        error_message = "{0} must be greater than or equal to {1}. value: ({2}).".format(property_name, minimum_number, property_value)

        if not self._is_not_none(property_name, property_value):
            return False

        if not self._is_integer(property_name, property_value):
            return False

        if minimum_number > property_value:
            self.logger.warning(error_message)
            return False

        return True
