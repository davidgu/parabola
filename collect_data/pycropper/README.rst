pyCropper
=========

pyCropper is a simple program that allows quick cropping for a large
collection of images. Built with pygame, it supports any file format that can
be read/saved by pygame, and it has been tested with .jpg, .pgm, .gif, .png,
.bmp, and .tif.

|logo|

Usage
-----
Place the ``pycropper.py`` file in a directory with the images to be cropped,
then run the following command::

    python pycropper.py

All images in that directory will be loaded into a list and displayed
sequentially. To crop an image, left-click to choose the top-left of the
rectangle over the portion of the image to which you want to crop.

Left-click again to set bottom-right of the rectangle, and the image will
immediately be cropped and saved. After the first left-click, you can
right-click to cancel the rectangle and start over.

Keyboard Input
--------------

* **Left-click**: (1) select top-left of cropping area, (2) select bottom-right,
  crop, and save image.
* **Right-click**: cancel crop area selection.
* **Left-arrow/Up-arrow**: Show previous image.
* **Right-arrow/Down-arrow**: Show next image.
* **ESC**: exit pycropper

License
-------

This code is available under the MIT license.

History
-------

This was originally written back in 2008, when I was playing around with
image analysis tools (e.g. opencv). I needed to quickly crop hundreds of images
after visually inspecting them, and tools like photoshop make this really
time consuming. Anyway, this is the result. You can also read the
`original blog post <https://bradmontgomery.net/blog/2008/05/20/pycropper/>`_.

.. |logo| image:: pycropper_logo.png
