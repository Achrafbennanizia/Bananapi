# Report: Banana Pi M5 Installation - PJMT Project

**Date:** December 26, 2025  
**Device:** Banana Pi M5 (BPI-M5)

---

## 📋 Summary

This report documents the complete process of installing and configuring an Armbian system on a Banana Pi M5 board, from downloading the image to enabling SSH and establishing remote access.

---

## 1. Image Selection and Download

### Image Used

| Parameter        | Value                          |
| ---------------- | ------------------------------ |
| **Distribution** | Armbian 25.5.1                 |
| **Base**         | Debian GNU/Linux 12 (Bookworm) |
| **Kernel**       | Linux 6.12.28-current-meson64  |
| **Architecture** | ARM64 (aarch64)                |
| **Image Type**   | Stable                         |

### Hardware Specifications (Banana Pi M5)

| Component  | Detail                              |
| ---------- | ----------------------------------- |
| **Board**  | Banana Pi M5                        |
| **Family** | meson-sm1                           |
| **SoC**    | Amlogic S905X3                      |
| **DTB**    | `amlogic/meson-sm1-bananapi-m5.dtb` |

---

## 2. Flashing the Image to USB/SD

### Method Used

The Armbian image was flashed to the storage media (USB/SD) using a flashing tool (e.g., balenaEtcher, dd, or Raspberry Pi Imager).

### Boot Configuration (`armbianEnv.txt`)

```
verbosity=1
console=both
overlay_prefix=meson
fdtfile=amlogic/meson-sm1-bananapi-m5.dtb
rootdev=UUID=30947093-6d3f-412f-8b20-56bfbe75698b
rootfstype=ext4
usbstoragequirks=0x2537:0x1066:u,0x2537:0x1068:u
```

---

## 3. Accessing EXT4 File System from macOS

### Problem

macOS does not natively support the EXT4 file system used by Linux/Armbian. To modify system files before boot, we need to mount the partition.

### Initial Attempt: Homebrew (Failed)

First attempt was to install ext4fuse via Homebrew:

```bash
brew install ext4fuse
```

**Error encountered:**

```
ext4fuse: Linux is required for this software.
libfuse@2: Linux is required for this software.
Error: ext4fuse: Unsatisfied requirements failed this build.
```

❌ Homebrew cannot install `ext4fuse` or `libfuse@2` on macOS - both require Linux. Manual compilation is needed.

### Solution: Manual Installation with macFUSE

#### Step 3.1: Install macFUSE

1. Download from: https://osxfuse.github.io/
2. Install the macFUSE package
3. **Restart the Mac** (required)
4. Approve system extensions in System Preferences > Security

#### Step 3.2: Compile ext4fuse Manually

```bash
# Clone the repository
git clone https://github.com/gerard/ext4fuse.git
cd ext4fuse

# Compile
make
```

#### Step 3.3: Mount the EXT4 Partition

```bash
# Create the mount point
mkdir ~/ext4_mount

# Identify the disk
diskutil list
# → Disk identified: /dev/disk4

# Mount the partition (sudo required)
sudo ./ext4fuse /dev/disk4 ~/ext4_mount -o allow_other
```

### Mount Verification

```bash
$ mount | grep ext4_mount
ext4fuse@macfuse0 on /Users/<username>/ext4_mount (macfuse, synchronous)
```

✅ **Status:** Mount successful

---

## 4. SSH Configuration Verification

### Objective

Ensure SSH starts automatically at boot to allow remote access to the Banana Pi M5.

### Verification Steps Performed

#### 4.1 Check SSH Service Enabled at Startup

We verified that SSH is enabled in systemd's multi-user target (boot):

```bash
$ ls -la ~/ext4_mount/etc/systemd/system/multi-user.target.wants/ | grep ssh
ssh.service -> /lib/systemd/system/ssh.service
```

✅ **SSH is enabled at startup** - The symlink exists, meaning the SSH daemon will start automatically when the system boots.

#### 4.2 Check SSH Service in systemd

```bash
$ ls -la ~/ext4_mount/etc/systemd/system/ | grep ssh
sshd.service -> /lib/systemd/system/ssh.service
```

✅ **SSH service is properly linked**

#### 4.3 SSHD Configuration (`/etc/ssh/sshd_config`)

We checked the SSH daemon configuration file for key settings:

| Parameter                | Value           | Status     | Description                          |
| ------------------------ | --------------- | ---------- | ------------------------------------ |
| `PermitRootLogin`        | `yes`           | ✅ Enabled | Allows root user to login via SSH    |
| `PasswordAuthentication` | `yes` (default) | ✅ Enabled | Allows password-based authentication |

### Result

**SSH is already configured and enabled by default in this Armbian image.** No modifications were required - the image comes with SSH ready to use.

---

## 5. Default Credentials

### Armbian Default Login

| Username | Password |
| -------- | -------- |
| `root`   | `1234`   |

### First Boot Wizard

On first boot, Armbian will prompt you to:

1. **Change the root password** (mandatory)
2. **Create a new regular user account**
3. **Set locale and timezone**

---

6. System Structure

### Root Partition Contents

### Contenu de la Partition Root

```
/
├── bin -> usr/bin
├── boot/
│   ├── Image -> vmlinuz-6.12.28-current-meson64
│   ├── armbianEnv.txt
│   ├── boot.scr
│   ├── dtb/
│   └── uInitrd
├── etc/
├── home/
├── lib -> usr/lib
├── root/
├── usr/
└── var/
```

---

## 7. SSH Connection

### Connecting to the Banana Pi M5

After booting the device, connect via SSH:

```bash
ssh root@192.168.178.33
```

### Troubleshooting: Connection Refused

If you get `Connection refused` error:

```bash
ssh: connect to host 192.168.178.33 port 22: Connection refused
```

**Possible causes:**

1. **Device still booting** - Wait 1-2 minutes for full boot
2. **SSH service not started yet** - First boot takes longer
3. **Wrong IP address** - Check your router for the correct IP
4. **Network issues** - Verify device is connected

**Diagnostic commands:**

```bash
# Check if device is reachable
ping 192.168.178.33

# Check if port 22 is open
nc -zv 192.168.178.33 22
```

### Successful Connection

Once connected, you'll see:

```
root@192.168.178.33's password:
```

Enter the default password: `1234`

---

## 8. Post-Installation Steps

1. **Safely eject the disk (before first boot):**

   ```bash
   sudo umount ~/ext4_mount
   ```

2. **Insert into the Banana Pi M5** and boot

3. **SSH Connection:**

   ```bash
   ssh root@<IP_ADDRESS>
   ```

   - Default password: `1234`

4. **Initial Armbian Configuration:**
   - Change root password (mandatory on first login)
   - Create standard user
   - Configure timezone and locales

---

## 9. Tools Used

| Tool           | Version | Usage                 |
| -------------- | ------- | --------------------- |
| macFUSE        | 5.1.3   | FUSE support on macOS |
| ext4fuse       | latest  | Mount EXT4 partitions |
| Terminal (zsh) | -       | System commands       |

---

## 10. References

- [Armbian Documentation](https://docs.armbian.com)
- [Banana Pi M5 Wiki](https://wiki.banana-pi.org/Banana_Pi_BPI-M5)
- [ext4fuse GitHub](https://github.com/gerard/ext4fuse)
- [macFUSE](https://osxfuse.github.io/)
- [Fix ext4fuse Issue #74](https://github.com/gerard/ext4fuse/issues/74#issuecomment-1374069541)

---

**Final Status:** ✅ Installation complete - SSH enabled and verified
