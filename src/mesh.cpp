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

    for (std::vector<Vertex>::iterator vertex_ptr = vertices.begin(); vertex_ptr < vertices.end(); vertex_ptr++)
    {
        out << "v "
            << vertex_ptr->pos[0] << " "
            << vertex_ptr->pos[1] << " "
            << vertex_ptr->pos[2] << " "
            << vertex_ptr->pos[3] << "\n";
    }

    for (std::vector<Face>::iterator face_ptr = faces.begin(); face_ptr < faces.end(); face_ptr++)
    {
        int indexarray[3];
        for (std::vector<Vertex>::iterator vertex_ptr = vertices.begin(); vertex_ptr < vertices.end(); vertex_ptr++)
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
    for (std::vector<Vertex>::iterator vertex_ptr = vertices.begin(); vertex_ptr < vertices.end(); vertex_ptr++)
    {
        Vector<4> new_pos = transformation_mat * (vertex_ptr->pos);
        vertex_ptr->Update_Position(new_pos);
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
    double t = INFINITY;
    Vector<3> colour;
    Vector<3> normal;
    for (std::vector<Face>::iterator face_iterator = faces.begin(); face_iterator < faces.end(); face_iterator++)
    {
        Plane face_plane(face_iterator->normal, face_iterator->vertices[0]->pos.GetAsVector3());
        std::shared_ptr<RayIntersect> face_intersect = face_plane.Intersect(ray);

        // Ray does not interect face plane i.e. ray is parrallel to the face
        // or itersect is behind another face
        if (!face_intersect || face_intersect->t > t)
        {
            continue;
        }

        Vector<3> intersect_pos = ray.Position(face_intersect->t);

        Vector<3> vertex_0 = face_iterator->vertices[0]->pos.GetAsVector3();
        Vector<3> vertex_1 = face_iterator->vertices[1]->pos.GetAsVector3();
        Vector<3> vertex_2 = face_iterator->vertices[2]->pos.GetAsVector3();

        // Get vectors between face verticies
        Vector<3> vector1 = vertex_1 - vertex_0;
        Vector<3> vector2 = vertex_2 - vertex_1;
        Vector<3> vector3 = vertex_0 - vertex_2;

        Vector<3> norm1 = CrossProduct(intersect_pos - vertex_0, vector1);
        Vector<3> norm2 = CrossProduct(intersect_pos - vertex_1, vector2);
        Vector<3> norm3 = CrossProduct(intersect_pos - vertex_2, vector3);

        // TODO check if another dot prod is needed
        if (norm1.DotProduct(norm2) >= 0 && norm1.DotProduct(norm3) >= 0 && norm2.DotProduct(norm3) >= 0)
        {
            t = face_intersect->t;
            colour = face_iterator->colour;
            normal = face_iterator->normal;
        }
    }

    if (t == INFINITY)
    {
        return nullptr;
    }

    std::shared_ptr<RayIntersect> intersect(new RayIntersect(t, colour, this, normal));
    return intersect;
}

void Mesh::SetColour(Vector<3> colour)
{
    for (std::vector<Face>::iterator face_iterator = faces.begin(); face_iterator < faces.end(); face_iterator++)
    {
        face_iterator->colour = colour;
    }
};

void Mesh::RecalculateNormals()
{
    for (std::vector<Face>::iterator face_iterator = faces.begin(); face_iterator < faces.end(); face_iterator++)
    {
        face_iterator->CalculateNormal();
    }
}