from PIL import Image
import numpy as np

def image_to_bitmap(image_path, threshold=128):
    """
    Convert an image to a bitmap representation of 1s and 0s.
    
    Parameters:
    - image_path: Path to the input image
    - threshold: Pixel intensity threshold (0-255) to convert to binary (default 128)
    
    Returns:
    - A string representation of the bitmap
    """
    # Open the image and convert to grayscale
    img = Image.open(image_path).convert('L')
    
    # Convert image to numpy array
    img_array = np.array(img)
    
    # Apply threshold to create binary image
    binary_array = (img_array > threshold).astype(int)
    
    # Create bitmap string representation
    height, width = binary_array.shape
    
    # First line is the size
    bitmap_lines = [f"{height} {width}"]
    
    # Add bitmap rows
    for row in binary_array:
        bitmap_lines.append(' '.join(map(str, row)))
    
    return '\n'.join(bitmap_lines)

# Example usage
def main():
    # Replace 'path/to/your/image.jpg' with the actual path to your image
    path = "/home/dzeniks/Documents/4k.jpg"
    
    bitmap_output = image_to_bitmap(path)
    
    # Print or save the bitmap
    # print(bitmap_output)
    
    # Save to a text file with same name as image
    with open(f"{path.split('/')[-1].split('.')[0]}.txt", 'w') as f:
        f.write(bitmap_output)

if __name__ == "__main__":
    main()