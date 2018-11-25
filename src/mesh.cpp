#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iterator>
#include <vector>
#include <string>
#include <algorithm>
#include <memory>
#include "vector.h"
#include "mesh.h"
#include "matrix.h"
#include "plane.h"

using namespace lin_alg;

// Adapted from https://stackoverflow.com/a/21153233
void Mesh::LoadObjectModel(const char *filename)
{
    std::ifstream in(filename, std::ifstream::in);
    if (!in)
    {
        std::cerr << "Cannot open " << filename << "\n";
        exit(1);
    }

    std::string line;
    std::vector<Vector<3>> vertex_normals;

    while (getline(in, line))
    {
        // check for vertex
        if (line.substr(0, 2) == "v ")
        {
            std::istringstream v(line.substr(2));
            double x, y, z;
            v >> x;
            v >> y;
            v >> z;
            vertices.push_back(*new Vertex(x, y, z));
        }
        // check for face
        else if (line.substr(0, 2) == "f ")
        {
            std::istringstream f(line.substr(2));
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
        // check for vertex normal
        else if (line.substr(0,3) == "vn ")
        {
            std::istringstream v(line.substr(3));
            double x, y, z;
            v >> x;
            v >> y;
            v >> z;
            vertex_normals.push_back({x, y, z});
        }
    }

    in.close();
};

void Mesh::ExportObjectModel(const char *filename, bool overwrite)
{
    if (overwrite)
    {
        return WriteToFile(filename, 1, true);
    };

    std::ifstream in(filename, std::ifstream::in);

    int startingVertexIndex = 1;

    if (in.good())
    {
        std::string line;
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

void Mesh::WriteToFile(const char *filename, const int startingVertexIndex, bool overwrite)
{
    std::_Ios_Openmode openmode = std::ofstream::out;

    if (!overwrite)
    {
        openmode = openmode | std::ofstream::app;
    }
    else
    {
        openmode = openmode | std::ofstream::trunc;
    }

    std::ofstream out(filename, openmode);

    if (startingVertexIndex > 1)
    {
        out << "\n";
    }

    out << std::fixed << std::setprecision(4);

    for (const Vertex &vertex : vertices)
    {
        out << "v "
            << vertex.pos[0] << " "
            << vertex.pos[1] << " "
            << vertex.pos[2] << " "
            << vertex.pos[3] << "\n";
    }

    for (const Face &face : faces)
    {
        int indexarray[3];
        for (std::size_t i = 0; i < vertices.size(); ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                if (face.vertices[j] == &vertices[i])
                {
                    indexarray[j] = i + startingVertexIndex;
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
    std::unique_ptr<Matrix<4>> trans_matrix = HomoTransMatrix4D(x, y, z);

    UpdateVertices(*trans_matrix);
}

void Mesh::Rotate(Axis3D a, double angle)
{
    std::unique_ptr<Matrix<4>> rot_matrix = HomoRotMatix4D(a, angle);

    UpdateVertices(*rot_matrix);
}

void Mesh::UpdateVertices(Matrix<4> &transformation_mat)
{
    for (Vertex &vertex : vertices)
    {
        Vector<4> new_pos = transformation_mat * (vertex.pos);
        vertex.Update_Position(new_pos);
    }
}

void Mesh::AddRotation(Axis3D a, double angle)
{
    std::unique_ptr<Matrix<4>> rot_matrix = HomoRotMatix4D(a, angle);

    Matrix<4> product = *rot_matrix * transformation_queue;

    transformation_queue = product;
}

void Mesh::AddTranslation(double x, double y, double z)
{
    std::unique_ptr<Matrix<4>> trans_matrix = HomoTransMatrix4D(x, y, z);

    Matrix<4> product = *trans_matrix * transformation_queue;

    transformation_queue = product;
}

std::shared_ptr<RayIntersect> Mesh::Intersect(Ray ray)
{
    std::shared_ptr<RayIntersect> closest = nullptr;

    for (Face &face : faces)
    {
        std::shared_ptr<RayIntersect> intersect = face.Intersect(ray);
        if (intersect && (!closest || intersect->t < closest->t))
        {
            closest = intersect;
        }
    }

    if (closest)
    {
        closest->object = this;
    }

    return closest;
}

void Mesh::SetColour(Vector<3> colour)
{
    for (Face &face : faces)
    {
        face.colour = colour;
    }
};

void Mesh::RecalculateNormals()
{
    for (Face &face : faces)
    {
        face.CalculateNormal();
    }
}