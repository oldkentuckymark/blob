import bpy
import csv
import os

# Set output directory (adjust as needed)
output_dir = "/home/mark/"

# Collect all mesh objects in the scene
mesh_objects = [obj for obj in bpy.context.scene.objects if obj.type == 'MESH']

if not mesh_objects:
    raise Exception("No mesh objects found in the scene!")

for obj in mesh_objects:
    # Build output path from mesh name, sanitizing spaces
    safe_name = bpy.path.clean_name(obj.name)
    output_path = os.path.join(output_dir, f"{safe_name}.csv")

    # Ensure mesh data is up to date
    mesh = obj.data
    mesh.calc_loop_triangles()

    # Try to get vertex colors
    color_layer = None
    if mesh.vertex_colors:
        color_layer = mesh.vertex_colors.active

    # Open a new CSV file for this mesh
    with open(output_path, mode='w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        # Header row
        #writer.writerow(["x", "y", "z", "r", "g", "b", "a"])

        vert_count = 0

        for tri in mesh.loop_triangles:
            for loop_index in tri.loops:
                vert_index = mesh.loops[loop_index].vertex_index
                vertex = mesh.vertices[vert_index]
                co = obj.matrix_world @ vertex.co  # world coordinates

                # Default color
                r = g = b = a = 1.0
                if color_layer:
                    color = color_layer.data[loop_index].color
                    r, g, b, a = color

                writer.writerow([co.x, co.z, co.y, r, g, b, a])
                vert_count += 1

    print(f"Exported '{obj.name}' → {output_path} ({vert_count} vertices)")

print(f"Done — {len(mesh_objects)} file(s) written to {output_dir}")
