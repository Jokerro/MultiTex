precision mediump float;
varying vec2 v_uvPos;
uniform sampler2D u_tex0;
uniform sampler2D u_tex1;
uniform sampler2D u_tex_mask;

void main()
{
	vec4 tex0 = texture2D(u_tex0, v_uvPos);
	vec4 tex1 = texture2D(u_tex1, v_uvPos);
	vec4 blTex = texture2D(u_tex_mask, v_uvPos);
	gl_FragColor =  (blTex.r * tex0 + blTex.b * tex1) / (blTex.r + blTex.b);
}
