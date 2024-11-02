# ramizbinary.py
driver_path = "ramiz.sys"
output_path = "driver_data.h"

with open(driver_path, "rb") as f:
    data = f.read()

with open(output_path, "w") as f:
    f.write("unsigned char driverData[] = {")
    f.write(",".join("0x{:02x}".format(b) for b in data))
    f.write("};\n")
    f.write("unsigned int driverData_len = {};".format(len(data)))

certificate_path = "UTKUBABAPRO.cer"
output_certificate_path = "certificate_data.h"

with open(certificate_path, "rb") as f:
    data = f.read()

with open(output_certificate_path, "w") as f:
    f.write("unsigned char certifcateData[] = {")
    f.write(",".join("0x{:02x}".format(b) for b in data))
    f.write("};\n")
    f.write("unsigned int certificateData_len = {};".format(len(data)))