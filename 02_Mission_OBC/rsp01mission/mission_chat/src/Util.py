"""
    文字列型をint型に変換する。
    int型ならintを返し、
    str型ならintにして返し、
    それ以外ならNoneを返す。
"""
def convertStringToInt(data_str):
    if type(data_str) is int:
        return data_str
    elif type(data_str) is str and data_str.isdigit():
        return int(data_str)
    elif type(data_str) is str:
        return data_str
    else:
        return None
