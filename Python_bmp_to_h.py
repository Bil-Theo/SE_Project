import os

def bmp_to_h(bmp_path, h_path):
    try:
        # Lire le fichier BMP brut
        with open(bmp_path, 'rb') as bmp_file:
            bmp_data = bmp_file.read()

        # Créer le fichier .h
        with open(h_path, 'w') as h_file:
            h_file.write(f"// Image convertie de {os.path.basename(bmp_path)}\n")
            h_file.write("#ifndef IMAGE_H\n#define IMAGE_H\n\n")
            h_file.write("unsigned char fond_bmp[] = {\n")

            # Écrire les données en format hexadécimal, 16 octets par ligne
            for i in range(0, len(bmp_data), 16):
                line = bmp_data[i:i+16]
                h_file.write("  " + ", ".join(f"0x{byte:02X}" for byte in line) + ",\n")

            h_file.write("};\n\n")
            h_file.write("#endif // IMAGE_H\n")

        print(f"Fichier .h généré avec succès : {h_path}")

    except Exception as e:
        print(f"Erreur lors de la conversion : {e}")

# Chemin de l'image BMP et destination du fichier .h
bmp_path = r"C:\\Users\\arthu\\Documents\\M1TDSI\\STATION_METEO\\soleil.bmp"
h_path = r"C:\\Users\\arthu\\Documents\\M1TDSI\\STATION_METEO\\soleil.h"

# Conversion
if __name__ == "__main__":
    if os.path.exists(bmp_path):
        bmp_to_h(bmp_path, h_path)
    else:
        print(f"Le fichier {bmp_path} n'existe pas. Veuillez vérifier le chemin.")
