from PIL import Image
import numpy as np

def image_to_bitmap(width, height):
    """
    Convert an image to a bitmap representation of 1s and 0s.
    
    Parameters:
    - image_path: Path to the input image
    - threshold: Pixel intensity threshold (0-255) to convert to binary (default 128)
    
    Returns:
    - A string representation of the bitmap
    """
    # Create array full of 1s
    binary_array = np.ones((width, height)).astype(int)
    
    # First line is the size
    bitmap_lines = [f"{height} {width}"]
    
    # Add bitmap rows
    for row in binary_array:
        bitmap_lines.append(' '.join(map(str, row)))
    
    return '\n'.join(bitmap_lines)

# Example usage
def main():
    # Replace 'path/to/your/image.jpg' with the actual path to your image
    bitmap_output = image_to_bitmap(16000, 16000)
    
    # Print or save the bitmap
    # print(bitmap_output)
    
    # Save to a text file with same name as image
    with open(f"gen_full_ones_16k.txt", 'w') as f:
        f.write(bitmap_output)

if __name__ == "__main__":
    main()