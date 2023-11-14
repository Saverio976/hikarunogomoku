#!/usr/bin/env python3

import tkinter as tk
from tkinter import filedialog
from tkinter import messagebox

class MapEditorApp:
    def __init__(self, root):
        self.root = root
        self.map_size = 20
        self.data_map = [['x' for _ in range(self.map_size)] for _ in range(self.map_size)]
        self.mask_map = [[0 for _ in range(self.map_size)] for _ in range(self.map_size)]
        self.score = tk.StringVar(value='50.0')
        self.description = tk.StringVar(value='Horizontal three with one open end')
        self.pattern = tk.StringVar(value='20 20')
        self.setup_ui()

    def setup_ui(self):
        # Set up the main frame
        main_frame = tk.Frame(self.root)
        main_frame.pack(fill=tk.BOTH, expand=True)

        # Data map frame
        data_map_frame = tk.LabelFrame(main_frame, text='Data Map')
        data_map_frame.grid(row=0, column=0, padx=10, pady=10, sticky='nsew')

        # Mask map frame
        mask_map_frame = tk.LabelFrame(main_frame, text='Mask Map')
        mask_map_frame.grid(row=0, column=1, padx=10, pady=10, sticky='nsew')

        # Data and mask canvases
        self.data_canvas = tk.Canvas(data_map_frame, width=400, height=400)
        self.mask_canvas = tk.Canvas(mask_map_frame, width=400, height=400)
        self.data_canvas.pack()
        self.mask_canvas.pack()

        self.draw_map(self.data_canvas, self.data_map, self.toggle_data_point)

        self.draw_map(self.mask_canvas, self.mask_map, self.toggle_mask_point)

        tk.Label(main_frame, text='Description:').grid(row=1, column=0)
        tk.Entry(main_frame, textvariable=self.description).grid(row=1, column=1, sticky='ew')

        tk.Label(main_frame, text='Pattern:').grid(row=2, column=0)
        tk.Entry(main_frame, textvariable=self.pattern).grid(row=2, column=1, sticky='ew')

        tk.Label(main_frame, text='Score:').grid(row=3, column=0)
        tk.Entry(main_frame, textvariable=self.score).grid(row=3, column=1, sticky='ew')

        save_button = tk.Button(main_frame, text='Save', command=self.save_to_file)
        save_button.grid(row=4, column=0, columnspan=2, pady=10)

        # Configure grid weights
        main_frame.columnconfigure(0, weight=1)
        main_frame.columnconfigure(1, weight=1)
        main_frame.rowconfigure(0, weight=1)

    def draw_map(self, canvas, map_data, click_handler):
        cell_width = 20
        cell_height = 20
        line_thickness = 1

        rect_width = cell_width - line_thickness
        rect_height = cell_height - line_thickness

        for i, row in enumerate(map_data):
            for j, val in enumerate(row):
                x1 = j * cell_width
                y1 = i * cell_height
                x2 = x1 + rect_width
                y2 = y1 + rect_height
                canvas.create_rectangle(x1, y1, x2, y2, fill='white', outline='gray', tags=f"cell_{i}_{j}")
                canvas.create_text((x1 + rect_width / 2, y1 + rect_height / 2), text=val)
        canvas.bind("<Button-1>", click_handler)

    def toggle_data_point(self, event):
        cell_width = 20
        cell_height = 20
        canvas = event.widget
        row = event.y // cell_height
        column = event.x // cell_width
        current_value = self.data_map[row][column]
        # Cycle between 'O', 'P', and 'x'
        if current_value == 'P':
            new_value = 'O'
        elif current_value == 'O':
            new_value = 'x'
        else:
            new_value = 'P'
        self.data_map[row][column] = new_value
        self.update_cell(canvas, row, column, new_value)

    def update_cell(self, canvas, row, column, value):
        cell_width = 20
        cell_height = 20
        x1 = column * cell_width
        y1 = row * cell_height
        x2 = x1 + cell_width
        y2 = y1 + cell_height
        # Use different colors for each value if you wish
        fill_color = 'white' if value == 'x' else 'green' if value == 'P' else 'red'
        canvas.create_rectangle(x1, y1, x2, y2, fill=fill_color, outline='gray', tags=f"cell_{row}_{column}")
        canvas.create_text((x1 + cell_width / 2, y1 + cell_height / 2), text=value, tags=f"text_{row}_{column}")

        # Delete the old text before creating new text
        canvas.delete(f"text_{row}_{column}")
        canvas.create_text((x1 + cell_width / 2, y1 + cell_height / 2), text=value, tags=f"text_{row}_{column}")

    def toggle_mask_point(self, event):
        cell_width = 20
        cell_height = 20
        canvas = event.widget
        row = event.y // cell_height
        column = event.x // cell_width
        current_value = self.mask_map[row][column]
        new_value = 0 if current_value != 0 else 1
        self.mask_map[row][column] = new_value
        self.update_mask_cell(canvas, row, column, new_value)

    def update_mask_cell(self, canvas, row, column, value):
        cell_width = 20
        cell_height = 20
        x1 = column * cell_width
        y1 = row * cell_height
        x2 = x1 + cell_width
        y2 = y1 + cell_height
        fill_color = 'gray' if value == 1 else 'white'
        canvas.create_rectangle(x1, y1, x2, y2, fill=fill_color, outline='gray', tags=f"cell_{row}_{column}")

    def save_to_file(self):
        data_map_str = '\n'.join([' '.join(row) for row in self.data_map])

        mask_str_list = []
        for i, row in enumerate(self.mask_map):
            for j, val in enumerate(row):
                if val == 1:
                    mask_str_list.append(f"# x y\n{j} {i}")
        mask_str = '\n'.join(mask_str_list)

        file_content = f'''desciption
{self.description.get()}

pattern
{self.pattern.get()}

data
{data_map_str}

mask
{mask_str}
endmask

score
{self.score.get()}
'''

        file_path = tk.filedialog.asksaveasfilename(
            defaultextension=".txt",
            filetypes=[("Text files", "*.txt"), ("All files", "*.*")],
        )

        if file_path:
            try:
                with open(file_path, 'w') as file:
                    file.write(file_content)
                messagebox.showinfo("Success", "The file has been saved successfully!")
            except Exception as e:
                messagebox.showerror("Error", f"An error occurred while saving the file: {e}")
        else:
            messagebox.showinfo("Cancelled", "Save operation has been cancelled")

# Here is the main function which will initialize the application and start the main event loop.

if __name__ == '__main__':
    root = tk.Tk()
    app = MapEditorApp(root)
    root.title("Map Editor App")
    root.geometry("850x500")  # Set the initial size of the window
    root.mainloop()  # Start the GUI event loop
