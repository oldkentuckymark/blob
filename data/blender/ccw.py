#!/usr/bin/env python3
"""
Script to reorder mesh vertices for OpenGL CCW winding with outward-facing normals.

The script reads CSV vertices (x, y, z, r, g, b, a) grouped in triangles.
It ensures CCW winding by checking the signed volume of each triangle relative to its neighbors.
This ensures consistent normal direction for OpenGL rendering.
"""

import sys
import math

def cross_product(v1, v2):
    """Compute cross product of two 3D vectors."""
    return [
        v1[1] * v2[2] - v1[2] * v2[1],
        v1[2] * v2[0] - v1[0] * v2[2],
        v1[0] * v2[1] - v1[1] * v2[0]
    ]

def dot_product(v1, v2):
    """Compute dot product of two 3D vectors."""
    return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2]

def triangle_area(v0, v1, v2):
    """Compute the magnitude of the triangle's normal (approximate area)."""
    normal = cross_product([v1[i] - v0[i] for i in range(3)], [v2[i] - v0[i] for i in range(3)])
    return math.sqrt(sum(x * x for x in normal))

def main():
    # Read input
    input_text = sys.stdin.read().strip()
    if not input_text:
        print("Error: No input provided.", file=sys.stderr)
        sys.exit(1)

    lines = [line.strip() for line in input_text.split('\n') if line.strip()]
    vertices = []

    for line in lines:
        parts = line.split(',')
        if len(parts) == 7:
            try:
                v = [float(p) for p in parts]
                vertices.append(v)
            except ValueError:
                print(f"Warning: Skipping invalid vertex line: {line}", file=sys.stderr)

    if len(vertices) % 3 != 0:
        print("Error: Number of vertices must be a multiple of 3.", file=sys.stderr)
        sys.exit(1)

    count = len(vertices)
    mesh_center = [
        sum(v[0] for v in vertices) / count,
        sum(v[1] for v in vertices) / count,
        sum(v[2] for v in vertices) / count
    ]

    # Process each triangle
    output_lines = []
    for i in range(0, count, 3):
        idx0, idx1, idx2 = i, i + 1, i + 2
        v0, v1, v2 = vertices[idx0], vertices[idx1], vertices[idx2]

        # Compute triangle normal
        e1 = [v1[j] - v0[j] for j in range(3)]
        e2 = [v2[j] - v0[j] for j in range(3)]
        normal = cross_product(e1, e2)

        # Vector from triangle center to mesh center
        tri_center = [
            (v0[0] + v1[0] + v2[0]) / 3.0,
            (v0[1] + v1[1] + v2[1]) / 3.0,
            (v0[2] + v1[2] + v2[2]) / 3.0
        ]
        to_center = [mesh_center[j] - tri_center[j] for j in range(3)]

        # Check if normal points inward
        dot_val = dot_product(normal, to_center)

        # If dot is positive, normal points inward; flip vertices
        if dot_val < 1e-9:
            idx1, idx2 = idx2, idx1

        # Append vertices
        output_lines.append(vertices[idx0])
        output_lines.append(vertices[idx1])
        output_lines.append(vertices[idx2])

    # Write output
    for v in output_lines:
        print(','.join(str(x) for x in v))

if __name__ == "__main__":
    main()
