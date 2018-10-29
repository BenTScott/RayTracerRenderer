#include "face.h"

void Face::CalculateNormal(){
    auto v1_v2_diff = vertices[0]->pos - vertices[1]->pos;
    auto v1_v3_diff = vertices[0]->pos - vertices[2]->pos;
    normal = lin_alg::Vector<4>::CrossProduct(v1_v2_diff, v1_v3_diff).GetAsVector3();
};