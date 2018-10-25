"""
pyCropper
---------

A program to quickly crop a large collection of images.

Instructions
------------

Left-click to start drawing the top-left corner.
Left-click again to set the bottom-right corner.
Left/Up-arrows: view previous image.
Right/Down-arrays: view next image.
ESC: quit.

"""
import os
import sys
import time

import pygame
from pygame import locals as C  # a bunch of constants

# --- Global Variables --------------------------------------------------------
supported_files = [".jpg", ".pgm", ".jpeg", ".gif", ".png", ".tif", ".bmp"]

screen_width = 1024 # width of the screen
screen_height = 7680  # height of the screen
resolution = (screen_width, screen_height)

files = []  # list of image files to crop
current_file_index = 0  # index into the current files list
reloadImage = True  # do we reload the image from the file

wait_time = 0.5  # how long to wait after cropping
current_rect = None  # currently drawn rectangle
crop_rect = None  # rectangle to which we'll crop
click_state = 0  # Stat of left-click:
                 # 1 means we started to draw a rect.
                 # 2 means we've finished drawing a rect,
                 # and need to crop and save the image.
#------------------------------------------------------------------------------


def load_image_list(directory):
    """Create a list of image filenames from a directory ignoring any
    subdirectories or files that start with a "."
    """
    images = os.listdir(directory)
    print('list of images: ', images)
    images.sort()  # sort the images in alpha-num order
    i = 0
    for image in images:
        filename, ext = os.path.splitext(image)
        if ext in supported_files:
            path = os.path.join(directory, image)
            if os.path.isfile(path) and image[0] != ".":
                images[i] = image
                print('keeping ', image)
                i = i + 1

    images = images[:i]  # make sure we don't have any duplicates (in case we
                         # skipped something like .svn)
    return images


def cropImage(im, left, top, right, bottom):
    """Crop the given image inside the given coordinates and return the cropped
    image.

    * ``im`` - a reference to a PIL image

    """
    cropbox = [left, top, right, bottom]
    tmp = im.crop(cropbox)
    return tmp


def PIL_to_surface(im):
    """ Convert a PIL image to a Pygame surface
        Return the pygame surface.
    """
    return pygame.image.fromstring(
        im.tostring(),
        im.size,
        im.mode,
        flipped=False
    )


def text_image(text, size, color, font_name="courier new"):
    """ Create a surface from the given text:
        text - the text to draw
        size - the size of the text (ie: 12, 24, 44, etc)
        color - an RGB tuple
        font_name - name of the font to use (default is "courier new")
    """
    font = pygame.font.SysFont(font_name, size, True)
    return font.render(text, True, color)


def draw_welcome(screen):
    """ a simple function to draw the hud at the top of the screen """
    screen.fill((0, 0, 0))
    logo = pygame.image.load("pycropper_logo.png")
    subfont = pygame.font.SysFont("courier new", 24, True)
    line2 = "when you need to do a lot of quick cropping."
    instructions = [
        "all images in the current directory will be loaded, ",
        "just click on the image to choose the top-left of the rectagle ",
        "over the portion of the image to which you want to crop.",
        "left-click again to crop the image to the drawn rectangle.",
        "right-click to cancel the rectangle and start over. ",
        "\npyCropper is available under the MIT license"
    ]
    line3 = "(press Space to continue)"
    line2_surface = subfont.render(line2, True, (255, 255, 255))
    line3_surface = subfont.render(line3, True, (255, 255, 255))

    pt = (100, 170)
    for l in instructions:
        s = subfont.render(l, True, (255, 255, 200))
        screen.blit(s, pt)
        pt = (pt[0], pt[1] + 15)

    screen.blit(logo, (100, 10))
    screen.blit(line2_surface, (100, 120))
    screen.blit(line3_surface, (200, 350))


def input(events):
    """ A function to handle keyboard/mouse/device input events. """
    global click_state, current_rect, crop_rect, current_file_index, \
        files, reloadImage

    for event in events:
        if (event.type == C.QUIT or
           (event.type == C.KEYDOWN and event.key == C.K_ESCAPE)):
            sys.exit()

        elif event.type == C.KEYDOWN and event.key == C.K_SPACE:
            return "ok"  # Just used for the Welcome screen

        elif (event.type == C.MOUSEBUTTONDOWN and event.button == 1 and
              click_state == 0):  # left mouse click
            click_state = 1  # Stat of left-click:
            # left, top, width, height
            current_rect = pygame.Rect(event.pos[0], event.pos[1], 1, 1)

        elif (event.type == C.MOUSEBUTTONDOWN and event.button == 1 and
              click_state == 1):  # left mouse click
            click_state = 2  # Stat of left-click:
            crop_rect = pygame.Rect(
                current_rect.left,
                current_rect.top,
                current_rect.width,
                current_rect.height
            )
            current_rect = None

        elif (event.type == C.MOUSEBUTTONDOWN and event.button == 3 and
              click_state == 1):  # right mouse click
            # Cancel the Rect
            current_rect = None
            click_state = 0

        elif event.type == C.MOUSEMOTION and click_state == 1:
            w = abs(current_rect.left - event.pos[0])
            h = abs(current_rect.top - event.pos[1])
            current_rect.width = w
            current_rect.height = h

        elif (event.type == C.KEYDOWN and
              (event.key == C.K_LEFT or event.key == C.K_UP)):
            click_state = 0
            current_file_index = (current_file_index - 1) % len(files)
            reloadImage = True

        elif (event.type == C.KEYDOWN and
              (event.key == C.K_RIGHT or event.key == C.K_DOWN)):
            click_state = 0
            current_file_index = (current_file_index + 1) % len(files)
            reloadImage = True


def main():
    global files, screen_width, screen_height, resolution, current_rect, \
           crop_rect, click_state, current_file_index, reloadImage, wait_time

    pygame.init()

    screen = pygame.display.set_mode(resolution)
    draw_welcome(screen)
    pygame.display.flip()
    files = load_image_list("./")  # load all files in the current directory.
    image = None

    while(input(pygame.event.get()) != "ok"):
        pass  # just wait till the user does something.

    if len(files) > 0:
        reloadImage = True
        screen.fill((0, 0, 0))  # erase the screen
    else:
        txt = "Could not find any supported files... Exiting."
        msg = text_image(txt, 24, (255, 255, 255))
        screen.fill((0, 0, 0))
        screen.blit(msg, (10, screen_height - 30))
        pygame.display.flip()
        time.sleep(3)
        sys.exit(0)

    current_file_index = 0
    while current_file_index < len(files):

        if reloadImage:
            image = pygame.image.load(files[current_file_index])
            screen.fill((0, 0, 0))  # erase the screen
            screen.blit(image, (0, 0))  # draw the image
            pygame.display.flip()
            reloadImage = False

        if click_state == 0:
            screen.fill((0, 0, 0))
            screen.blit(image, (0, 0))  # draw the image
            txt = "Showing Image #%s: %s" % (
                current_file_index,
                files[current_file_index]
            )
            msg = text_image(txt, 24, (255, 255, 255))
            screen.blit(msg, (10, screen_height - 30))

        if click_state == 1:
            # draw the current rect
            screen.fill((0, 0, 0))
            screen.blit(image, (0, 0))  # draw the image
            txt = "Showing Image #%s: %s" % (
                current_file_index,
                files[current_file_index]
            )
            msg = text_image(txt, 24, (255, 255, 255))
            screen.blit(msg, (10, screen_height - 30))
            pygame.draw.rect(screen, (255, 0, 0), current_rect, 1)

        elif click_state == 2:  # crop the image, save it, and load the next
            # crop the image
            crop_image = pygame.Surface((crop_rect.width, crop_rect.height))
            crop_image.blit(image, (0, 0), crop_rect)
            screen.fill((0, 0, 0))
            screen.blit(crop_image, (0, 0))
            # save to file
            filename, ext = os.path.splitext(files[current_file_index])
            f = "crop_%s%s" % (current_file_index, ext)
            try:
                pygame.image.save(crop_image, f)
            finally:
                print('----------> saved as ', f)

            msg = text_image("Saved as " + f, 24, (255, 255, 255))
            screen.blit(msg, (10, screen_height - 30))

            current_file_index += 1  # advance to next file
            click_state = 0  # reset our click state

            # load the next file (if there is one)
            if current_file_index < len(files):
                reloadImage = True
            else:
                image = None
            pygame.display.flip()
            time.sleep(wait_time)  # pause just a second

        input(pygame.event.get())  # handle any input events.
        pygame.display.flip()  # update the window


if __name__ == "__main__":
    main()
