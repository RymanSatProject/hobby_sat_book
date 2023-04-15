# coding: utf-8

import sqlite3
from contextlib import closing
from datetime import date
from pathlib import Path

from settings import PHOTO_DIR_FOR_CLASSIFICATION, DB_FILE_PATH, LABELS, RESULTS, MAX_IDS_NUM, MAX_SKIP_BLANK_NUM


class PicDb(object):

    def __init__(self):
        self._db_file_path = str(DB_FILE_PATH)
        self._create_table()

    def _create_table(self):
        with closing(sqlite3.connect(self._db_file_path)) as conn:
            c = conn.cursor()
            create_table = '''
                CREATE TABLE IF NOT EXISTS pics (
                    id int,
                    sub_id int,
                    label int,
                    percent int,
                    by_all boolean,
                    primary key(id, sub_id)
                )'''
            c.execute(create_table)
            conn.commit()

    def get_max_pic_id_set_by_all(self):
        with closing(sqlite3.connect(self._db_file_path)) as conn:
            c = conn.cursor()
            sql = '''
                SELECT 
                  id, 
                  MAX(sub_id) 
                FROM 
                  pics 
                WHERE 
                  id = (
                    SELECT 
                      MAX(id) 
                    FROM 
                      pics 
                    WHERE 
                      by_all = 1
                  ) 
                  AND by_all = 1
                '''
            return c.execute(sql).fetchone()

    def save_classification_result(self, pic_id, pic_sub_id, label, percent, by_all):
        _pic_sub_id = pic_sub_id if pic_sub_id is not None else 0
        with closing(sqlite3.connect(self._db_file_path)) as conn:
            c = conn.cursor()
            sql = '''REPLACE INTO pics(id, sub_id, label, percent, by_all) VALUES(?, ?, ?, ?, ?)'''
            c.execute(sql, (pic_id, _pic_sub_id, label, percent, by_all))
            conn.commit()

    def get_classification_result(self, pic_id, pic_sub_id):
        _pic_sub_id = pic_sub_id if pic_sub_id is not None else 0
        with closing(sqlite3.connect(self._db_file_path)) as conn:
            c = conn.cursor()
            sql = '''SELECT label, percent FROM pics WHERE id = ? and sub_id = ?'''
            result = c.execute(sql, (pic_id, _pic_sub_id)).fetchone()
            if result is None:
                return None, None
            return result

    def get_pic_id_sets_filtered_by_label(self, label):
        with closing(sqlite3.connect(self._db_file_path)) as conn:
            c = conn.cursor()
            sql = '''
                SELECT
                  id, 
                  sub_id 
                FROM 
                  pics 
                WHERE 
                  label = ? 
                ORDER BY 
                  id DESC, 
                  sub_id DESC 
                LIMIT {max_ids_num}
            '''.format(max_ids_num=MAX_IDS_NUM)
            result = c.execute(sql, (label,)).fetchall()
            if result is None:
                return []
            return result

    @classmethod
    def reset_all_data(cls):
        if DB_FILE_PATH.exists():
            DB_FILE_PATH.rename(Path('_'.join([str(DB_FILE_PATH), date.today().strftime('%Y%m%d')])))


class PicManager(object):

    def __init__(self, all_or_continuous=False):
        if all_or_continuous:
            PicDb.reset_all_data()
        self._db = PicDb()
        self._all_or_continuous = all_or_continuous

    def next_pic(self):
        """
        id: 0 からカウントアップ
        sub_id: -1 -> None -> 0 -> 1...以降、カウントアップ
        """
        max_pic_id, max_pic_sub_id = self._db.get_max_pic_id_set_by_all()
        next_pic_id, next_pic_sub_id = (0, -1) if max_pic_id is None else (max_pic_id, max_pic_sub_id)
        blank_counter_for_id = 0
        blank_counter_for_sub_id = 0
        while True:
            if blank_counter_for_id > MAX_SKIP_BLANK_NUM:
                yield None
                break
            next_pic_sub_id = self._get_next_sub_id(next_pic_sub_id)
            pic = Pic(next_pic_id, next_pic_sub_id)
            if not pic.exists():
                blank_counter_for_sub_id += 1
                if blank_counter_for_sub_id > MAX_SKIP_BLANK_NUM:
                    next_pic_sub_id = -1
                    next_pic_id += 1
                    blank_counter_for_id += 1
                    blank_counter_for_sub_id = 0
                continue
            else:
                blank_counter_for_id = 0
                blank_counter_for_sub_id = 0
                yield pic

    @classmethod
    def _get_next_sub_id(cls, current_sub_id):
        if current_sub_id == -1:
            return None
        if current_sub_id is None:
            return 0
        return current_sub_id + 1

    def get_good_pic_ids(self):
        pic_ids = []
        pic_id_sets = self._db.get_pic_id_sets_filtered_by_label(LABELS.index('good'))
        for pic_id_set in pic_id_sets:
            pic_id = int(pic_id_set[0])
            pic_sub_id = int(pic_id_set[1])
            if pic_sub_id == 0 and not Pic(pic_id, pic_sub_id).exists():
                pic_ids.append('{0:03d}'.format(pic_id))
            else:
                pic_ids.append('{0:03d}_{1:04d}'.format(pic_id, pic_sub_id))
        return pic_ids


class Pic(object):

    FILE_NAME_PREFIX = 'image_'

    def __init__(self, pic_id, pic_sub_id=None):
        self._pic_id = int(pic_id)
        self._pic_sub_id = int(pic_sub_id) if pic_sub_id is not None else None
        self.image_file_path = self._convert_from_pic_id_to_image_file_path(self._pic_id, self._pic_sub_id)
        self._label = None
        self._percent = None
        self._db = PicDb()

    def need_to_classify(self):
        if self._label is not None and self._percent is not None:
            return False
        self._label, self._percent = self._db.get_classification_result(self._pic_id, self._pic_sub_id)
        return self._label is None or self._percent is None

    def save_classification_result(self, label, percentage, by_all=False):
        self._db.save_classification_result(self._pic_id, self._pic_sub_id, label, percentage, by_all)

    def get_classification_result(self):
        if self.need_to_classify():
            return RESULTS['getPicCateg']['no_classification'], RESULTS['getPicCateg']['no_classification']
        return self._label, self._percent

    @classmethod
    def _convert_from_pic_id_to_image_file_path(cls, pic_id, pic_sub_id):
        """
        普通に撮影: image_000.jpg
        連射撮影: image_000_0000.jpg, image_000_0001.jpg, image_000_0002.jpg, ...
        """
        if pic_sub_id is None:
            file_name = '{0}{1:03d}.jpg'.format(cls.FILE_NAME_PREFIX, int(pic_id))
        else:
            file_name = '{0}{1:03d}_{2:04d}.jpg'.format(cls.FILE_NAME_PREFIX, int(pic_id), int(pic_sub_id))
        return PHOTO_DIR_FOR_CLASSIFICATION / file_name

    def exists(self):
        return self.image_file_path.exists()

    def __repr__(self):
        return self.__str__()

    def __str__(self):
        return str(self.image_file_path)
