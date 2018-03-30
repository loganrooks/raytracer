from PIL import Image

img = Image.open("car_cube_map.jpg")

area = (0, 1000, 999, 1999)
cropped_img = img.crop(area)
cropped_img.save("cube_car_map/side1.bmp")

area = (1000, 1000, 1999, 1999)
cropped_img = img.crop(area)
cropped_img.save("cube_car_map/side2.bmp")

area = (2000, 1000, 2999, 1999)
cropped_img = img.crop(area)
cropped_img.save("cube_car_map/side3.bmp")

area = (3000, 1000, 3999, 1999)
cropped_img = img.crop(area)
cropped_img.save("cube_car_map/side4.bmp")

area = (1000, 0, 1999, 999)
cropped_img = img.crop(area)
cropped_img.save("cube_car_map/side5.bmp")

area = (1000, 2000, 1999, 2999)
cropped_img = img.crop(area)
cropped_img.save("cube_car_map/side6.bmp")
