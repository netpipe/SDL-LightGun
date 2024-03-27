from PIL import Image, ImageDraw, ImageFont

# Define the size of each character cell
cell_width = 20
cell_height = 20

# Define the number of characters per row in the font image
chars_per_row = 16

# Define the characters to include in the font image (ASCII printable characters)
characters = "".join(chr(i) for i in range(0, 127)) + " " * chars_per_row * 2  # Including two blank lines

# Create a new image for the font
font_image = Image.new("RGB", (cell_width * chars_per_row, cell_height * ((len(characters) - 1) // chars_per_row + 1)), "white")

# Load a font
font = ImageFont.load_default()

# Create a draw object
draw = ImageDraw.Draw(font_image)

# Draw each character onto the font image
for i, char in enumerate(characters):
    char_x = (i % chars_per_row) * cell_width
    char_y = (i // chars_per_row) * cell_height

    # Calculate the position to center the character within the cell
    char_pos = ((cell_width - font.getsize(char)[0]) // 2, (cell_height - font.getsize(char)[1]) // 2)

    draw.text((char_x + char_pos[0], char_y + char_pos[1]), char, fill="black", font=font)

# Save the font image
font_image.save("font.png")

print("Font image generated successfully!")

