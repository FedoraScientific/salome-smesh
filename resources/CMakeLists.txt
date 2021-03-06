# Copyright (C) 2007-2014  CEA/DEN, EDF R&D, OPEN CASCADE
#
# Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
# CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

SET(SMESH_RESOURCES_FILES
  delete.png
  mesh_add_sub.png
  mesh_algo_hexa.png
  mesh_algo_mefisto.png
  mesh_algo_quad.png
  mesh_algo_regular.png
  mesh_algo_tetra.png
  mesh_angle.png
  mesh_area.png
  mesh_aspect.png
  mesh_aspect_3d.png
  mesh_biquad_quadrangle.png
  mesh_biquad_triangle.png
  mesh_choose.png
  mesh_choose_all.png
  mesh_clear.png
  mesh_compute.png
  mesh_diagonal.png
  mesh_edit.png
  mesh_hexa.png
  mesh_hypo_area.png
  mesh_hypo_length.png
  mesh_hypo_segment.png
  mesh_hypo_volume.png
  mesh_hypo_edit.png
  mesh_plus.png
  mesh_minus.png
  mesh_info.png
  advanced_mesh_info.png
  standard_mesh_info.png
  mesh_elem_info.png
  mesh_equal_edge.png
  mesh_equal_face.png
  mesh_equal_node.png
  mesh_equal_volume.png
  mesh_whatis.png
  mesh_init.png
  mesh_length.png
  mesh_length_2d.png
  mesh_find_elem_by_point.png
  mesh_free_edges.png
  mesh_free_edges_2d.png
  mesh_free_nodes.png
  mesh_max_element_length_2d.png
  mesh_max_element_length_3d.png
  mesh_multi_edges.png
  mesh_multi_edges_2d.png
  mesh_line_n.png
  mesh_line.png
  mesh_move_node.png
  mesh_move_without_node.png
  mesh_octahedron.png
  mesh_orientation.png
  mesh.png
  mesh_polygon.png
  mesh_polyhedron.png
  mesh_pyramid_n.png
  mesh_pyramid.png
  mesh_quad_n.png
  mesh_quad.png
  mesh_quadrangle_quadpref.png
  mesh_quadrangle_quadpref_reversed.png
  mesh_quadrangle_reduced.png
  mesh_quadrangle_standard.png
  mesh_quadrangle_triapref.png
  mesh_rem_element.png
  mesh_rem_node.png
  mesh_rem_orphan_nodes.png
  mesh_shading.png
  mesh_shrink.png
  mesh_skew.png
  mesh_taper.png
  mesh_tetra.png
  mesh_tree_algo_hexa.png
  mesh_tree_algo_mefisto.png
  mesh_tree_algo.png
  mesh_tree_algo_0D.png
  mesh_tree_algo_quad.png
  mesh_tree_algo_regular.png
  mesh_tree_algo_tetra.png
  mesh_tree_hypo_area.png
  mesh_tree_hypo_length.png
  mesh_tree_hypo.png
  mesh_tree_hypo_segment.png
  mesh_tree_hypo_volume.png
  mesh_tree_mesh.png
  mesh_tree_importedmesh.png
  mesh_tree_mesh_warn.png
  mesh_triangle_n.png
  mesh_triquad_hexahedron.png
  mesh_triangle.png
  mesh_update.png
  mesh_vertex_n.png
  mesh_vertex.png
  mesh_0D_elem.png
  mesh_0D_on_all_nodes.png
  mesh_volume_3d.png
  bare_border_volume.png
  bare_border_face.png
  over_constrained_volume.png
  over_constrained_face.png
  mesh_wireframe.png
  mesh_points.png
  mesh_wrap.png
  mesh_group.png
  mesh_tree_group.png
  mesh_tree_group_on_filter.png
  mesh_edit_group.png
  mesh_make_group.png
  mesh_groups_from_gemetry.png
  mesh_union2tri.png
  mesh_uniontri.png
  mesh_cutquad.png
  mesh_smoothing.png
  mesh_renumbering_nodes.png
  mesh_renumbering_elements.png
  mesh_extrusion.png
  mesh_extrusionpath.png
  mesh_revolution.png
  ModuleMesh.png
  mesh_unionGroups.png
  mesh_intersectGroups.png
  mesh_cutGroups.png
  mesh_deleteGroups.png
  mesh_translation_vector.png
  mesh_translation_points.png
  mesh_rotation.png
  mesh_symmetry_point.png
  mesh_symmetry_axis.png
  mesh_symmetry_plane.png
  mesh_sew_freeborders.png
  mesh_sew_conform_freeborders.png
  mesh_sew_bordertoside.png
  mesh_sew_sideelements.png
  mesh_merge_nodes.png
  mesh_merge_elements.png
  select1.png
  open.png
  mesh_pattern.png
  mesh_pentahedron.png
  pattern_sample_2d.png
  pattern_sample_3D.png
  mesh_add.png
  mesh_remove.png
  mesh_quad_edge.png
  mesh_quad_triangle.png
  mesh_quad_quadrangle.png
  mesh_quad_tetrahedron.png
  mesh_quad_pyramid.png
  mesh_quad_pentahedron.png
  mesh_quad_hexahedron.png
  mesh_conv_to_quad.png
  mesh_tree_hypo_layers_distribution.png
  mesh_tree_algo_radial_prism.png
  mesh_tree_algo_radial_quadrangle_1D2D.png
  mesh_tree_algo_existing_2D.png
  mesh_tree_algo_prism.png
  mesh_tree_algo_projection_2d.png
  mesh_hypo_source_edge.png
  mesh_hypo_source_3d.png
  mesh_hypo_layer_distribution.png
  mesh_hypo_source_face.png
  mesh_tree_hypo_source_face.png
  mesh_tree_hypo_source_edge.png
  mesh_tree_hypo_source_3d_shape.png
  mesh_tree_hypo_projection_3d.png
  mesh_tree_hypo_projection_2d.png
  mesh_build_compound.png
  copy_mesh.png
  mesh_node_to_point.png
  mesh_tree_mesh_partial.png
  mesh_extractGroup.png
  mesh_precompute.png
  mesh_2d_from_3d.png
  mesh_free_faces.png
  scale.png
  scale_along_axes.png
  split_into_tetra.png
  mesh_duplicate_nodes.png
  mesh_duplicate_nodes_with_elem.png
  mesh_duplicate_elem_only.png
  mesh_duplicate_group_boundary.png
  mesh_bounding_box.png
  mesh_hypo_viscous_layers.png
  mesh_tree_hypo_viscous_layers.png
  mesh_min_dist.png
  reorient_faces_point.png
  reorient_faces_face.png
  reorient_faces_volume.png
  mesh_ball.png
  mesh_measure_basic_props.png
  mesh_measure_length.png
  mesh_measure_area.png
  mesh_measure_volume.png
)

INSTALL(FILES ${SMESH_RESOURCES_FILES} DESTINATION ${SALOME_SMESH_INSTALL_RES_DATA})

SALOME_CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/SalomeApp.xml.in ${CMAKE_CURRENT_BINARY_DIR}/SalomeApp.xml INSTALL ${SALOME_SMESH_INSTALL_RES_DATA})
SALOME_CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/StdMeshers.xml.in ${CMAKE_CURRENT_BINARY_DIR}/StdMeshers.xml INSTALL ${SALOME_SMESH_INSTALL_RES_DATA})
SALOME_CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/SMESHCatalog.xml.in ${CMAKE_CURRENT_BINARY_DIR}/SMESHCatalog.xml INSTALL ${SALOME_SMESH_INSTALL_RES_DATA})
