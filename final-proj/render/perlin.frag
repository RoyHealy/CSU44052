#version 330 core

in vec2 position;

uniform vector<vector<vec2>> grid;

out float finalColor;

static float interpolate(float a0, float a1, float w) {
    return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
}

static float dotGridGradient(int x0, int y0, pos) {
    // float dist = distance(vec2(x0,y0), pos);
    return dot(grid[x0][y0], pos);
}
 
static float perlin(vec2 pos) {
    // 4 grid locations
    int x0 = (int)pos.x, y0 = (int)pos.y;
    int x1 = x0+1,   y1 = y0+1;
    // weights for interpolating
    float sx = x - (float)x0;
    float sy = y - (float)y0;
    
    float n0 = dotGridGradient(x0, y0, pos);
    float n1 = dotGridGradient(x1, y0, pos);
    float ix0 = interpolate(n0, n1, sx);

    n0 = dotGridGradient(x0, y1, pos);
    n1 = dotGridGradient(x1, y1, pos);
    float ix1 = interpolate(n0, n1, sx);

    return interpolate(ix0,ix1,sy);
}

void main()
{
	finalColor = perlin(coordinate);
}
