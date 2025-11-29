import sys
from PIL import Image
import os

def convert_image_to_c_array(image_path):
    # Verificam daca fisierul exista
    if not os.path.exists(image_path):
        print(f"EROARE: Fisierul '{image_path}' nu a fost gasit!")
        return

    try:
        img = Image.open(image_path)
    except Exception as e:
        print(f"EROARE: Nu pot deschide imaginea. Motiv: {e}")
        return

    # Verificarea dimensiunilor (Strict 128x64)
    width, height = img.size
    if width != 128 or height != 64:
        print(f"\n!!! EROARE CRITICA !!!")
        print(f"Imaginea are {width}x{height} pixeli.")
        return

    # Conversie la monocrom (1-bit)
    img = img.convert('1') 
    
    # Accesam pixelii
    pixels = img.load()
    
    buffer = []

    print(f"\nProcesez imaginea: {image_path} (128x64) ...")

    # Algoritmul de scanare pentru OLED SSD1306 (Horizontal/Page Addressing)
    for page in range(8): # 0-7
        for col in range(128): # 0-127
            byte = 0
            # Construim un byte din 8 pixeli verticali
            for bit in range(8):
                y = page * 8 + bit
                x = col
                
                # Verificam pixelul.
                pixel_val = pixels[x, y]
                
                # Daca pixelul e alb (aprins), setam bitul corespunzator
                if pixel_val > 0:
                    byte |= (1 << bit)
            
            buffer.append(byte)

    # Generarea textului pentru codul C
    c_code = f"/* Imagine generata din: {os.path.basename(image_path)} */\n"
    c_code += f"uint8_t img_{os.path.splitext(os.path.basename(image_path))[0]}[] = {{\n"
    
    for i, byte in enumerate(buffer):
        if i % 16 == 0:
            c_code += "    "
        c_code += f"0x{byte:02X}, "
        if (i + 1) % 16 == 0:
            c_code += "\n"
            
    c_code = c_code.rstrip(", \n") + "\n};"

    # Afisare rezultat
    print("\nSUCCES! Copiază codul:\n")
    print("-" * 80)
    print(c_code)
    print("-" * 80)

#   main
if __name__ == "__main__":

    if len(sys.argv) > 1:
        path = sys.argv[1]
    else:
        path = "NXP_Panda_opt.png" 
        
    convert_image_to_c_array(path)