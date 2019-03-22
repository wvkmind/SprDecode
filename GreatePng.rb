require 'chunky_png'




image = ChunkyPNG::Image.from_rgb_stream(124,131, File.read('./data/SkillEff.spr0.555.124x131.data'))
image.compose!(ChunkyPNG::Image.new(124, 131, ChunkyPNG::Color.rgba(255, 255,255, 128)))

image.save('./decodedata/ChangeAl.spr0.png')
