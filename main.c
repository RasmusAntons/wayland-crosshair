#include <stdio.h>
#include <wayland-client.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#include "wlr-layer-shell-unstable-v1-client-protocol.h"
#include "xdg-shell-client-protocol.h"
#include "crosshair.h"
#include "shm.h"

static struct wl_display *display;
static struct wl_compositor *compositor;
static struct wl_shm *shm;
static void *shm_data = NULL;
static struct xdg_wm_base *xdg_wm_base;
static struct zwlr_layer_shell_v1 *layer_shell;
static struct wl_output *wl_output;
struct wl_surface *wl_surface;
struct zwlr_layer_surface_v1 *layer_surface;
static uint32_t width = 32, height = 32;


static void
handle_global(void *data, struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version)
{
	if (strcmp(interface, wl_compositor_interface.name) == 0) {
		compositor = wl_registry_bind(registry, name, &wl_compositor_interface, 1);
	} else if (strcmp(interface, wl_shm_interface.name) == 0) {
		shm = wl_registry_bind(registry, name, &wl_shm_interface, 1);
	} else if (strcmp(interface, zwlr_layer_shell_v1_interface.name) == 0) {
		layer_shell = wl_registry_bind(registry, name, &zwlr_layer_shell_v1_interface,
		                               version < 4 ? version : 4);
	} else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
		xdg_wm_base = wl_registry_bind(registry, name, &xdg_wm_base_interface, 1);
	}
}

static void handle_global_remove(void *data, struct wl_registry *registry, uint32_t name)
{

}

static const struct wl_registry_listener registry_listener = {
		.global = handle_global,
		.global_remove = handle_global_remove,
};

static struct wl_buffer *create_buffer()
{
	int stride = width * 4;
	int size = stride * height;

	int fd = create_shm_file(size);
	if (fd < 0) {
		fprintf(stderr, "creating a buffer file for %d B failed: %m\n", size);
		return NULL;
	}

	shm_data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (shm_data == MAP_FAILED) {
		fprintf(stderr, "mmap failed: %m\n");
		close(fd);
		return NULL;
	}

	struct wl_shm_pool *pool = wl_shm_create_pool(shm, fd, size);
	struct wl_buffer *buffer = wl_shm_pool_create_buffer(pool, 0, width, height,
	                                                     stride, WL_SHM_FORMAT_ARGB8888);
	wl_shm_pool_destroy(pool);

	// MagickImage is from cat.h
	memcpy(shm_data, MagickImage, size);
	return buffer;
}

int main()
{
	char *namespace = "crosshair";
	display = wl_display_connect(NULL);
	if (display == NULL) {
		fprintf(stderr, "Failed to create display\n");
		return 1;
	}
	struct wl_registry *registry = wl_display_get_registry(display);
	wl_registry_add_listener(registry, &registry_listener, NULL);
	wl_display_roundtrip(display);

	if (compositor == NULL) {
		fprintf(stderr, "wl_compositor not available\n");
		return 1;
	}
	if (shm == NULL) {
		fprintf(stderr, "wl_shm not available\n");
		return 1;
	}
	if (layer_shell == NULL) {
		fprintf(stderr, "layer_shell not available\n");
		return 1;
	}

	wl_surface = wl_compositor_create_surface(compositor);
	assert(wl_surface);
	struct wl_region *input_region = wl_compositor_create_region(compositor);
	wl_surface_set_input_region(wl_surface, input_region);
	layer_surface = zwlr_layer_shell_v1_get_layer_surface(layer_shell, wl_surface, wl_output,
	                                                      ZWLR_LAYER_SHELL_V1_LAYER_OVERLAY, namespace);
	zwlr_layer_surface_v1_set_size(layer_surface, width, height);
	zwlr_layer_surface_v1_set_anchor(layer_surface, 0);
	wl_surface_commit(wl_surface);
	wl_display_roundtrip(display);

	struct wl_buffer *buffer = create_buffer();
	if (buffer == NULL) {
		return EXIT_FAILURE;
	}

	wl_surface_attach(wl_surface, buffer, 0, 0);
	wl_surface_commit(wl_surface);

	while (wl_display_dispatch(display) != -1) {
		// This space intentionally left blank
	}

	return 0;
}
