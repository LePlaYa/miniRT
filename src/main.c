#include "../inc/minirt.h"

// int	print_mrt(t_mrt *m)
// {
// 	printf("\nLIGHT\n{\n");
// 	printf("id : [%c]\n", m->l->id);
// 	printf("cor : [%f][%f][%f]\n", m->l->cor[0], m->l->cor[1], m->l->cor[2]);
// 	printf("brit : [%f]\n}\n", m->l->brit);
// 	printf("\nSPHERE\n{\n");
// 	printf("id : [%c]\n", m->sp[0]->id);
// 	printf("cor : [%f][%f][%f]\n", m->sp[0]->cor[0], m->sp[0]->cor[1], m->sp[0]->cor[2]);
// 	printf("rad : [%f]\n", m->sp[0]->rad);
// 	printf("rgb : [%d][%d][%d]\n}\n", m->sp[0]->r, m->sp[0]->g, m->sp[0]->b);
// 	return (1);
// }

void	calc(t_mrt *mrt)
{
	t_data	img;
	int	y;
	int	x;
	double	**scr;

	scr = scream(mrt->cam);
	img.img = mlx_new_image(mrt->mlx, WDTH, HGHT);
	img.addr = mlx_get_data_addr(img.img, &img.bits_per_pixel, &img.line_length, &img.endian);
	y = 0;
	while (y < HGHT)
	{
		x = 0;
		while (x < WDTH)
		{
			nachfolger(x, y, mrt, scr, &img);
			x++;
		}
		y++;
	}
	mrt->img = img.img;
}

// Trace me baby one more time
void	debug(t_mrt *mrt)
{
	int	x;
	int	y;
	int	i;
	double	d;
	double	*ray;
	double	**scr;
	double	*norm;
	double	*impact;

	mlx_mouse_get_pos(mrt->mlx, mrt->win, &x, &y);
	scr = scream(mrt->cam);
	printf("ray trough x %i, y %i\n", x, y);
	ray = single_ray(x - (WDTH/2), y - (HGHT/2), mrt->cam, scr);
	i = 0;
	while (mrt && mrt->obj && mrt->obj[i] && mrt->obj[i]->id)
	{
		d = 0;
		impact = ray_alloc(mrt->cam->cor[0], mrt->cam->cor[1], mrt->cam->cor[2]);
		addto(impact, ray);
		if (mrt->obj[i]->id == 'S')
		{
			d = ROUND_ERROR * hit_sphere(mrt->obj[i]->cor, mrt->obj[i]->rad, mrt->cam->cor, ray);
			norm = connect(impact, mrt->obj[i]->cor);
		}
		if (mrt->obj[i]->id == 'P')
		{
			d = ROUND_ERROR * plane_intercept(mrt, ray, mrt->obj[i]);
			norm = ray_alloc(mrt->obj[i]->v_o[0], mrt->obj[i]->v_o[1], mrt->obj[i]->v_o[2]);
		}
		if (d)
		{
			resize(ray, d);
			printf("KIND %C	, at a distance of %lf\nInput Colors R%d G%d B%d\n", mrt->obj[i]->id, d, mrt->obj[i]->r, mrt->obj[i]->g, mrt->obj[i]->b);
			double	*light;
			double	bright;
			impact = ray_alloc(mrt->cam->cor[0], mrt->cam->cor[1], mrt->cam->cor[2]);
			addto(impact, ray);
			light = connect(impact, mrt->l->cor);
			bright = (angle(light, norm)) * 1000 * mrt->l->brit;
			printf("impact at	%lf %lf %lf\n", impact[0], impact[1], impact[2]);
			printf("light per at	 %lf %lf %lf\n", light[0], light[1], light[2]);
			printf("brightness factor %lf\n", bright);
		}
		i++;
	}
	// i = 0;
	// while (mrt && mrt->cy && mrt->cy[i])
	// {
	// 	d = cylinder_intersect(mrt->cy[i]->cor, mrt->cy[i]->rad, mrt->cy[i]->hght, mrt->cam->cor, ray);
	// 	if (d)
	// 		printf("cylinder hit	%i, at a distance of %i\n", i, d);
	// 	i++;
	// }
}


int	render(t_mrt *mrt)
{
	mlx_put_image_to_window(mrt->mlx, mrt->win, mrt->img, 0, 0);
	return (0);
}

int	end(t_mrt *mrt)
{
	//exit functions
	if (mrt)
		exit(0);
	return (0);
}

int	key_hook(int key, t_mrt *mrt)
{
	if (key == 65307)
		end(mrt);
	if (key == 100)
		debug(mrt);
	return (0);
}

int	main(int argc, char **argv)
{
	t_mrt	mrt;

	if (argc != 2)
	{
		printf("Error\nIncorrect number of Args\n");
		return (1);
	}
	if (input(&mrt, argv[1]))
		return (printf("ERROR INPUT\n"));
	mrt.mlx = mlx_init();
	mrt.win = mlx_new_window(mrt.mlx, WDTH, HGHT, "I wanna be Tracer");
	calc(&mrt);
	mlx_mouse_show(mrt.mlx, mrt.win);
	mlx_loop_hook(mrt.mlx, render, &mrt);
	mlx_key_hook(mrt.win, key_hook, &mrt);
	mlx_hook(mrt.win, 33, (1L << 17), end, &mrt);
	mlx_loop(mrt.mlx);
	rt_exit(&mrt);
	return (0);
}
