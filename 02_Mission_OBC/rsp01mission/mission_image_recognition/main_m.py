# coding: utf-8

import pathlib
import random
import shlex
import subprocess
import numpy as np
from PIL import Image, ImageEnhance, ImageFilter
from datetime import datetime
from math import sqrt


MOVIES_DIR = 'img/learning/movies/set5'
TMP_IMG_DIR = 'img/learning/tmp_images'
RSP_IMG = 'img/learning/rsp.png'
GOOD_IMG_DIR = 'img/learning/good'
BAD_IMG_DIR = 'img/learning/bad'
BLACK_IMG = 'img/learning/black.png'

MOVIE_RATE = 1  # １秒間を何枚の画像にするか
BIG_COUNT = 2  # 画像を分割する際に、何倍のサイズに拡大した後に分割するか
TRIMMING_WIDTH_COUNT = 10  # 拡大した画像の横分割数
TRIMMING_HEIGHT_COUNT = 6  # 拡大した画像の縦分割数
# TRIMMED_GOOD_INDEXES = [1, 7, 13, 19, 25, 31, 37, 43, 49, 55]  # set1: 001 - 006
# TRIMMED_GOOD_INDEXES = [0, 6, 12, 18, 24, 30, 36, 42, 48, 54]  # set2: 007 - 015
# TRIMMED_GOOD_INDEXES = []  # set3: 016
# TRIMMED_GOOD_INDEXES = [3, 9, 15, 21, 27, 33, 39, 45, 51, 57]  # set4
TRIMMED_GOOD_INDEXES = [5, 11, 17, 23, 29, 35, 41, 47, 53, 59]  # set5...使う画像自信も bad なので注意
BAD_RATE = 0.97  # bad は数が多いので間引く（ランダムで 97% 除く）
BLACK_RATE = 0.99  # black を適当に混ぜる率（1%

RSP_IMG_SIZE_RATE = 0.4  # ハメコミする rsp の大きさ率（背景の縦との比率）
TOP_SPACE_SIZE = 30  # rsp はちょっとした目に配置するので、そのための位置設定値

# common data
rsp_img = Image.open(RSP_IMG)
black_img = Image.open(BLACK_IMG)

rsp_good_images_set = [('normal', rsp_img),
                       ('brighter2', rsp_img.point(lambda x: x * 2)),
                       ('darker07', ImageEnhance.Brightness(rsp_img).enhance(0.7))]

rsp_bad_images_set = [('brighter40', rsp_img.point(lambda x: x * 4)),
                      ('darker03', ImageEnhance.Brightness(rsp_img).enhance(0.3))]


def get_fish_xn_yn(source_x, source_y, source_radius, k):
    """
    Get normalized x, y pixel coordinates from the original image and return normalized
    x, y pixel coordinates in the destination fished image.
    """
    return source_x / (1 + k * (source_radius ** 2)), source_y / (1 + k * (source_radius ** 2))


def get_unfish_xn_yn(source_x, source_y, source_radius, k):
    """
    Get normalized x, y pixel coordinates from the Fished image and return normalized
    x, y pixel coordinates in the destination un-fished image
    """
    return source_x / (1 - k * (source_radius ** 2)), source_y / (1 - k * (source_radius ** 2))


def fish(img, get_xn_yn_func, distortion_coefficient=-0.5, distortion_radius=0):
    """
    :type img: numpy.ndarray
    :param get_xn_yn_func: The function to use to get new pixel coordiantes from given pixel coordinates.
                           One function will 'move' pixels to create the fisheye effect, and another function
                           will 'move' the pixels in a 'fished' image back to their original location.
    :param distortion_coefficient: The amount of distortion to apply.
    :param distortion_radius: The radius in which the effect should be applied (normalized value).
                              0 means - apply effect to all pixels.
    :return: numpy.ndarray - the image with applied effect.
    """

    k = distortion_coefficient

    # prepare array for dst image
    dstimg = np.zeros_like(img)

    # floats for calculations
    w, h = float(img.shape[0]), float(img.shape[1])

    # easier calculation if we traverse x, y in dst image
    for x in range(len(dstimg)):
        for y in range(len(dstimg[x])):

            # normalize x and y to be in interval of [-1, 1]
            xnd, ynd = float((2 * x - w) / w), float((2 * y - h) / h)

            # get xn and yn distance from normalized center
            rd = sqrt(xnd ** 2 + ynd ** 2)

            # don't disturt outside given radius (if given)
            if rd > distortion_radius > 0:
                continue

            xdu, ydu = get_xn_yn_func(xnd, ynd, rd, k)

            # convert the normalized distorted xdn and ydn back to image pixels
            xu, yu = int(((xdu + 1) * w) / 2), int(((ydu + 1) * h) / 2)

            # if new pixel is in bounds copy from source pixel to destination pixel
            if 0 <= xu < img.shape[0] and 0 <= yu < img.shape[1]:
                dstimg[x][y] = img[xu][yu]

    return dstimg


def filter_fish_eyes(img):
    _img = np.asarray(img)
    img_undistorted = fish(_img, get_fish_xn_yn, distortion_coefficient=-0.5)  # (720, 1280, 3)
    x, y, _ = img_undistorted.shape
    _x = int(x * 0.2)
    _y = int(y * 0.2)
    _img_undistorted = img_undistorted[_x: x - _x][:, _y: y - _y]
    return Image.fromarray(_img_undistorted)


def add_black(img, y):
    """
    y: 0.....全部真っ黒
       100...下が 70% 真っ黒
       300...下が 30% 真っ黒
       -700..上が 70% 真っ黒
       -900..上が 30% 真っ黒
    """
    _img = img.copy()
    _img.paste(black_img, (0, y))
    return _img


def add_black_randomly(img):
    return add_black(img, random.choice([0, 100, 300, -700, -900]))


def convert_img2np(img):
    return np.asarray(img).astype("f").transpose(2, 0, 1) / 128 - 1.0


def convert_np2img(_np):
    array = np.asarray(((_np + 1) * 128).astype("i").transpose(1, 2, 0))
    return Image.fromarray(np.uint8(array))


def noisy(image, noise_typ):
    """
    'gauss'     Gaussian-distributed additive noise.
    'poisson'   Poisson-distributed noise generated from the data.
    's&p'       Replaces random pixels with 0 or 1.
    'speckle'   Multiplicative noise using out = image + n*image,where
                n is uniform noise with specified mean & variance.
    """
    _image = convert_img2np(image)
    row, col, ch = _image.shape
    if noise_typ == "gauss":
        mean = 0
        var = 0.1
        sigma = var**0.5
        gauss = np.random.normal(mean, sigma, (row, col, ch))
        gauss = gauss.reshape(row, col, ch)
        noised_image = _image + gauss
    elif noise_typ == "s&p":
        s_vs_p = 0.5
        amount = 0.004
        noised_image = np.copy(_image)

        # Salt mode
        num_salt = np.ceil(amount * _image.size * s_vs_p)
        coords = [np.random.randint(0, i - 1, int(num_salt)) for i in _image.shape]
        noised_image[coords] = 1

        # Pepper mode
        num_pepper = np.ceil(amount * _image.size * (1. - s_vs_p))
        coords = [np.random.randint(0, i - 1, int(num_pepper)) for i in _image.shape]
        noised_image[coords] = 0
    elif noise_typ == "poisson":
        vals = len(np.unique(_image))
        vals = 2 ** np.ceil(np.log2(vals))
        noised_image = np.random.poisson(_image * vals) / float(vals)
    elif noise_typ == "speckle":
        gauss = np.random.randn(row, col, ch)
        gauss = gauss.reshape(row, col, ch)
        noised_image = _image + _image * gauss
    else:
        noised_image = _image
    return convert_np2img(noised_image)


def convert_movies_to_images():
    for movie_file_path in pathlib.Path(MOVIES_DIR).glob('*.mp4'):
        cmd = 'ffmpeg -i "{}" -r {} "{}/{}_%06d.png"'.format(movie_file_path, MOVIE_RATE,
                                                             TMP_IMG_DIR, movie_file_path.stem)
        subprocess.Popen(shlex.split(cmd), stdout=subprocess.PIPE).wait()


def get_back_image_set(img):
    """
    good or bad, img
    """
    # back_images = [('good', img)]
    back_images = []  # set5 用
    for brightness in [1, 1.2, 0.7]:
        back_images = back_images + get_trimmed_images(img, 0, TRIMMING_WIDTH_COUNT, TRIMMING_HEIGHT_COUNT,
                                                       TRIMMED_GOOD_INDEXES, brightness)
    for brightness in [2, 0.3]:
        back_images = back_images + get_trimmed_images(img, 0, TRIMMING_WIDTH_COUNT, TRIMMING_HEIGHT_COUNT,
                                                       [], brightness)
    for brightness in [1, 1.5, 0.7, 2, 0.3]:
        back_images = back_images + get_trimmed_images(img, 90, TRIMMING_HEIGHT_COUNT, TRIMMING_WIDTH_COUNT,
                                                       [], brightness)
        back_images = back_images + get_trimmed_images(img, 180, TRIMMING_WIDTH_COUNT, TRIMMING_HEIGHT_COUNT,
                                                       [], brightness)
        back_images = back_images + get_trimmed_images(img, 270, TRIMMING_HEIGHT_COUNT, TRIMMING_WIDTH_COUNT,
                                                       [], brightness)
    return back_images


def get_trimmed_images(ori_img, rotate, width_count, height_count, trimmed_good_indexes, brightness):
    rotated_img = ori_img.rotate(rotate, expand=True)
    rotated_img = rotated_img.point(lambda x: x * brightness)
    trimmed_images = []
    big_img = rotated_img.resize((rotated_img.size[0] * BIG_COUNT, rotated_img.size[1] * BIG_COUNT))
    delta_width = int(float(big_img.size[0] - ori_img.size[0]) / width_count)
    delta_height = int(float(big_img.size[1] - ori_img.size[1]) / height_count)
    index = 0
    for w in xrange(width_count):
        for h in xrange(height_count):
            trimmed_img = big_img.crop((delta_width * w, delta_height * h,
                                        delta_width * w + ori_img.size[0], delta_height * h + ori_img.size[1]))
            good_or_not = 'good' if index in trimmed_good_indexes else 'bad'
            trimmed_images.append((good_or_not, trimmed_img))
            index += 1
    return trimmed_images


def paste_rsp_image(back_img, _rsp_img):
    b_width = back_img.size[0]
    b_height = back_img.size[1]
    r_height = int(b_height * RSP_IMG_SIZE_RATE)
    r_width = int(_rsp_img.size[0] * (float(r_height) / _rsp_img.size[1]))
    _rsp_img = _rsp_img.resize((r_width, r_height))
    back_img.paste(_rsp_img,
                   (int((b_width - r_width) / 2), int(((b_height - r_height) / 2) + TOP_SPACE_SIZE)),
                   _rsp_img)  # mask
    return back_img


def set_up():
    for _dir in [MOVIES_DIR, TMP_IMG_DIR, GOOD_IMG_DIR, BAD_IMG_DIR]:
        path = pathlib.Path(_dir)
        if not path.exists():
            pathlib.Path(_dir).mkdir(parents=True)


def run():
    set_up()
    convert_movies_to_images()
    for img_path in pathlib.Path(TMP_IMG_DIR).glob('*.png'):
        print(datetime.now(), img_path)
        img = Image.open(str(img_path))
        back_image_set = get_back_image_set(img)
        for prefix, _rsp_img in rsp_good_images_set:
            for i, (good_or_bad, back_img) in enumerate(back_image_set):
                if good_or_bad == 'bad' and random.random() <= BAD_RATE:
                    continue
                pasted_img = paste_rsp_image(back_img, _rsp_img)
                pasted_img = filter_fish_eyes(pasted_img)
                if random.random() >= BAD_RATE:
                    filtered_img = pasted_img.filter(ImageFilter.GaussianBlur(10))
                    filtered_img.save(BAD_IMG_DIR + '/{}_{}_{}{}'.format(prefix + '_b',
                                                                         img_path.stem, str(i), img_path.suffix))
                    if random.random() >= BAD_RATE:
                        noised_img = noisy(filtered_img, 'gauss')
                        noised_img.save(BAD_IMG_DIR + '/{}_{}_{}{}'.format(prefix + '_bn',
                                                                           img_path.stem, str(i), img_path.suffix))
                        if random.random() >= BLACK_RATE:
                            blacked_img = add_black_randomly(noised_img)
                            blacked_img.save(BAD_IMG_DIR + '/{}_{}_{}{}'.format(prefix + '_bnb2',
                                                                                img_path.stem, str(i), img_path.suffix))
                    if random.random() >= BLACK_RATE:
                        blacked_img = add_black_randomly(filtered_img)
                        blacked_img.save(BAD_IMG_DIR + '/{}_{}_{}{}'.format(prefix + '_bb2',
                                                                            img_path.stem, str(i), img_path.suffix))
                if random.random() >= BAD_RATE:
                    noised_img = noisy(pasted_img, 'gauss')
                    noised_img.save(BAD_IMG_DIR + '/{}_{}_{}{}'.format(prefix + '_n',
                                                                       img_path.stem, str(i), img_path.suffix))
                    if random.random() >= BLACK_RATE:
                        blacked_img = add_black_randomly(noised_img)
                        blacked_img.save(BAD_IMG_DIR + '/{}_{}_{}{}'.format(prefix + '_nb2',
                                                                            img_path.stem, str(i), img_path.suffix))
                if random.random() >= BLACK_RATE:
                    blacked_img = add_black_randomly(pasted_img)
                    blacked_img.save(BAD_IMG_DIR + '/{}_{}_{}{}'.format(prefix + '_b2',
                                                                        img_path.stem, str(i), img_path.suffix))
                fin_img_dir = GOOD_IMG_DIR if good_or_bad == 'good' else BAD_IMG_DIR
                pasted_img.save(fin_img_dir + '/{}_{}_{}{}'.format(prefix, img_path.stem, str(i), img_path.suffix))
        for prefix, _rsp_img in rsp_bad_images_set:
            for i, (_, back_img) in enumerate(back_image_set):
                if random.random() <= BAD_RATE:
                    continue
                pasted_img = paste_rsp_image(back_img, _rsp_img)
                pasted_img = filter_fish_eyes(pasted_img)
                if random.random() >= BAD_RATE:
                    filtered_img = pasted_img.filter(ImageFilter.GaussianBlur(10))
                    filtered_img.save(BAD_IMG_DIR + '/{}_{}_{}{}'.format(prefix + '_b',
                                                                         img_path.stem, str(i), img_path.suffix))
                    if random.random() >= BAD_RATE:
                        noised_img = noisy(filtered_img, 'gauss')
                        noised_img.save(BAD_IMG_DIR + '/{}_{}_{}{}'.format(prefix + '_bn',
                                                                           img_path.stem, str(i), img_path.suffix))
                        if random.random() >= BLACK_RATE:
                            blacked_img = add_black_randomly(noised_img)
                            blacked_img.save(BAD_IMG_DIR + '/{}_{}_{}{}'.format(prefix + '_bnb2',
                                                                                img_path.stem, str(i), img_path.suffix))
                    if random.random() >= BLACK_RATE:
                        blacked_img = add_black_randomly(filtered_img)
                        blacked_img.save(BAD_IMG_DIR + '/{}_{}_{}{}'.format(prefix + '_bb2',
                                                                            img_path.stem, str(i), img_path.suffix))
                    continue
                if random.random() >= BAD_RATE:
                    noised_img = noisy(pasted_img, 'gauss')
                    noised_img.save(BAD_IMG_DIR + '/{}_{}_{}{}'.format(prefix + '_n',
                                                                       img_path.stem, str(i), img_path.suffix))
                    if random.random() >= BLACK_RATE:
                        blacked_img = add_black_randomly(noised_img)
                        blacked_img.save(BAD_IMG_DIR + '/{}_{}_{}{}'.format(prefix + '_bb2',
                                                                            img_path.stem, str(i), img_path.suffix))
                    continue
                if random.random() >= BLACK_RATE:
                    blacked_img = add_black_randomly(pasted_img)
                    blacked_img.save(BAD_IMG_DIR + '/{}_{}_{}{}'.format(prefix + '_bb2',
                                                                        img_path.stem, str(i), img_path.suffix))
                    continue
                pasted_img.save(BAD_IMG_DIR + '/{}_{}_{}{}'.format(prefix, img_path.stem, str(i), img_path.suffix))


if __name__ == '__main__':
    run()
