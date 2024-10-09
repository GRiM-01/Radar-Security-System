import tkinter as tk
from tkinter import messagebox
import serial

ser = serial.Serial('COM3', 9600, timeout=1)

PASSWORD_ARM = "1234"
PASSWORD_DISARM = "5678"

def send_command(command):
    try:
        ser.write(command.encode())
        response = ser.readline().decode().strip()
        return response
    except serial.SerialException:
        return "Error: Arduino not connected."

def create_arm_button(master, area_num):
    arm_command = str(area_num)
    btn = tk.Button(master, text=f"Area {area_num} Arm", command=lambda a=arm_command: arm_disarm_area(a), state=tk.DISABLED, bg='#2E2E2E', fg='#FFFFFF')  # Dark background, white text
    btn.pack(side=tk.TOP, fill=tk.X, padx=5, pady=5)
    return btn

def create_disarm_button(master, area_num):
    disarm_command = str(area_num + 3)
    btn = tk.Button(master, text=f"Area {area_num} Disarm", command=lambda d=disarm_command: arm_disarm_area(d), state=tk.DISABLED, bg='#2E2E2E', fg='#FFFFFF')  # Dark background, white text
    btn.pack(side=tk.TOP, fill=tk.X, padx=5, pady=5)
    return btn

def check_password(entry, password, buttons):
    entered_password = entry.get()
    entry.delete(0, tk.END)  # Clear the entry after checking

    if entered_password == password:
        for button in buttons:
            button['state'] = tk.NORMAL
        if password == PASSWORD_ARM:
            notification_label.config(text="Armed buttons activated.")
        elif password == PASSWORD_DISARM:
            notification_label.config(text="Disarm buttons activated.")
    else:
        notification_label.config(text="Incorrect password. Please try again.")

def arm_disarm_area(area_command):
    try:
        response = send_command(area_command)
        area_num = int(area_command[-1]) 
        if "Armed" in response:
            notification_label.config(text=f"Area {area_num} Armed.")
        elif "Disarmed" in response:
            notification_label.config(text=f"Area {area_num} Disarmed.")
    except ValueError:
        notification_label.config(text="Error: Invalid area command.")

def quit_application():
    confirm_quit = messagebox.askyesno("Quit", "Are you sure you want to quit?")
    if confirm_quit:
        root.destroy()

def show_help():
    help_message = "Emergency Contacts:\n\nPolice: 10111\nAmbulance: 10177\nSystem Support: 22139454@dut4life.ac.za"
    messagebox.showinfo("Help", help_message)

if __name__ == "__main__":
    root = tk.Tk()
    root.title("Security System Control")
    root.configure(bg='#2E2E2E')  # Dark background

    arm_frame = tk.Frame(root, bg='#2E2E2E') 
    arm_frame.pack(side=tk.LEFT, padx=10, pady=10, fill=tk.Y)

    disarm_frame = tk.Frame(root, bg='#2E2E2E')  
    disarm_frame.pack(side=tk.RIGHT, padx=10, pady=10, fill=tk.Y)

    arm_buttons = []
    disarm_buttons = []

    for area_num in range(1, 4):
        arm_button = create_arm_button(arm_frame, area_num)
        disarm_button = create_disarm_button(disarm_frame, area_num)

        arm_buttons.append(arm_button)
        disarm_buttons.append(disarm_button)

    password_frame = tk.Frame(root, bg='#2E2E2E')  
    password_frame.pack(pady=10)

    password_label = tk.Label(password_frame, text="Enter Password:", font=('Helvetica', 12), bg='#2E2E2E', fg='#FFFFFF')  # Dark background, white text
    password_label.pack(side=tk.LEFT, padx=5)

    password_entry = tk.Entry(password_frame, show="*", font=('Helvetica', 12), bg='#2E2E2E', fg='#FFFFFF') 
    password_entry.pack(side=tk.LEFT, padx=5)

    arm_password_button = tk.Button(root, text="Activate Arm Buttons", command=lambda: check_password(password_entry, PASSWORD_ARM, arm_buttons), font=('Helvetica', 10), bg='#444444', fg='#FFFFFF')  # Darker button color, white text
    arm_password_button.pack(pady=5)

    disarm_password_button = tk.Button(root, text="Activate Disarm Buttons", command=lambda: check_password(password_entry, PASSWORD_DISARM, disarm_buttons), font=('Helvetica', 10), bg='#444444', fg='#FFFFFF')  
    disarm_password_button.pack(pady=5)

    help_button = tk.Button(root, text="Help", command=show_help, font=('Helvetica', 10), bg='#444444', fg='#FFFFFF')  
    help_button.pack(side=tk.LEFT, anchor=tk.SW, padx=10, pady=10)

    quit_button = tk.Button(root, text="Quit", command=quit_application, font=('Helvetica', 10), bg='#444444', fg='#FFFFFF')  
    quit_button.pack(side=tk.RIGHT, anchor=tk.SE, padx=10, pady=10)

    notification_label = tk.Label(root, text="", font=('Helvetica', 10), bg='#2E2E2E', fg='#FFFFFF')  
    notification_label.pack(side=tk.BOTTOM, anchor=tk.S, pady=10)

    root.mainloop()