#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iterator>
#include <vector>
#include <string>
#include <algorithm>
#include <memory>
#include "mesh.h"
#include "matrix.h"

// Adapted from https://stackoverflow.com/a/21153233
// TODO: Fix obj reading for face lines
void Mesh::LoadObjectModel(const char *filename)
{
    ifstream in(filename, ifstream::in);
    if (!in)
    {
        cerr << "Cannot open " << filename << "\n";
        exit(1);
    }

    string line;

    while (getline(in, line))
    {
        // check for vertex
        if (line.substr(0, 2) == "v ")
        {
            istringstream v(line.substr(2));
            double x, y, z;
            v >> x;
            v >> y;
            v >> z;
            vertices.push_back(*new Vertex(x, y, z));
        }
        // check for face
        else if (line.substr(0, 2) == "f ")
        {
            istringstream f(line.substr(2));
            int v1, v2, v3;
            f >> v1;
            f >> v2;
            f >> v3;
            Face *face = new Face(&vertices[v1 - 1], &vertices[v2 - 1], &vertices[v3 - 1]);
            faces.push_back(*face);
            vertices[v2 - 1].faces.push_back(face);
            vertices[v3 - 1].faces.push_back(face);
            vertices[v1 - 1].faces.push_back(face);
        }
    }

    in.close();
};

void Mesh::ExportObjectModel(const char *filename)
{
    ifstream in(filename, ifstream::in);

    int startingVertexIndex = 1;

    if (in.good())
    {
        string line;
        while (getline(in, line))
        {
            // check for vertex
            if (line.substr(0, 2) == "v ")
            {
                ++startingVertexIndex;
            }
        }
    }

    in.close();

    WriteToFile(filename, startingVertexIndex);
}

void Mesh::WriteToFile(const char *filename, const int startingVertexIndex)
{
    ofstream out(filename, ofstream::out | ofstream::app);

    if (startingVertexIndex > 1)
    {
        out << "\n";
    }

    out << std::fixed << std::setprecision(4);

    for (vector<Vertex>::iterator vertex_ptr = vertices.begin(); vertex_ptr < vertices.end(); vertex_ptr++)
    {
        out << "v "
            << vertex_ptr->pos[0] << " "
            << vertex_ptr->pos[1] << " "
            << vertex_ptr->pos[2] << " "
            << vertex_ptr->pos[3] << "\n";
    }

    for (vector<Face>::iterator face_ptr = faces.begin(); face_ptr < faces.end(); face_ptr++)
    {
        int indexarray[3];
        for (vector<Vertex>::iterator vertex_ptr = vertices.begin(); vertex_ptr < vertices.end(); vertex_ptr++)
        {
            for (int i = 0; i < 3; ++i)
            {
                if (face_ptr->vertices[i] == &(*vertex_ptr))
                {
                    indexarray[i] = std::distance(vertices.begin(), vertex_ptr) + startingVertexIndex;
                }
            }
        }

        out << "f "
            << indexarray[0] << " "
            << indexarray[1] << " "
            << indexarray[2] << "\n";
    }

    out.close();
}

void Mesh::Translate(double x, double y, double z)
{
    std::unique_ptr<Matrix<4>> trans_matrix = Matrix<4>::HomoTransMatrix4D(x, y, z);

    UpdateVertices(*trans_matrix);
}

void Mesh::Rotate(Matrix<4>::Axis3D a, double angle)
{
    std::unique_ptr<Matrix<4>> rot_matrix = Matrix<4>::HomoRotMatix4D(a, angle);

    UpdateVertices(*rot_matrix);
}

void Mesh::UpdateVertices(Matrix<4> &transformation_mat)
{
    for (vector<Vertex>::iterator vertex_ptr = vertices.begin(); vertex_ptr < vertices.end(); vertex_ptr++)
    {
        double *new_pos = transformation_mat * (vertex_ptr->pos);
        vertex_ptr->Update_Position(new_pos);
        delete new_pos;
    }
}

void Mesh::AddRotation(Matrix<4>::Axis3D a, double angle)
{
    std::unique_ptr<Matrix<4>> rot_matrix = Matrix<4>::HomoRotMatix4D(a, angle);

    Matrix<4> product = *rot_matrix * transformation_queue;

    transformation_queue = product;
}

void Mesh::AddTranslation(double x, double y, double z)
{
    std::unique_ptr<Matrix<4>> trans_matrix = Matrix<4>::HomoTransMatrix4D(x, y, z);

    Matrix<4> product = *trans_matrix * transformation_queue;

    transformation_queue = product;
}