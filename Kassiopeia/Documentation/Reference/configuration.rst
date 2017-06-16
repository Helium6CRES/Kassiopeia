Configuring Your Own Simulation
*******************************

This section gives a brief overview of what you need to know to configure your own *Kassiopeia*
simulation.

Overview and Units
------------------

The configuration of *Kassiopeia* is done in three main sections, plus an optional
fourth. These are the ``<messages>`` section which describes global message verbosity, the ``<geometry>`` section
which describes the system geometry and its extensions, and the ``<kassiopeia>`` section which
contains the elements needed for the particle tracking simulation. The optional fourth section
relates to the specification of a VTK or ROOT window for the visualization of aspects of a completed simulation.
A complete simulation file can thus be broken down into something which looks like the following::

    <messages>
        <!-- specification of the messaging configuration here -->
    </messages>

    <geometry>
        <!-- specification of the geometry and geometry extensions here -->
    </geometry>

    <kassiopeia>
        <!-- specification of the simulation elements and parameters here -->
    </kassiopeia>

    <!-- optional VTK window (requires VTK) -->
    <vtk_window>
        <!-- specification of the VTK window display here -->
    </vtk_window>

    <!-- optional ROOT window (requires ROOT) -->
    <root_window>
        <!-- specification of a ROOT display window here
    </root_window>

The XML based interface of *Kassiopeia* allows for a very flexible way in which to modify and configure
particle tracking simulations. Throughout the geometry, field, and simulation configuration
all physical quantities are specified
using MKS and their derived units. The exception to this rule is energy, which is specified
using electron volts (eV). It should be noted that XML elements are parsed in order, and elements
which are referenced by other elements must be declared before their first use.

XML Parsing and Features
------------------------

The document language used for describing a Kassiopeia simulation configuration is based on standard
XML, but has been augmented with several additional features to aid in complicated or repetitive tasks.
For a full description of this language and its features, see [1].

Variables
~~~~~~~~~

A local variable may be defined with a particular value
(integer,floating point type, string, etc.) with the following syntax::

    <define name="my_variable" value="1.3e-5"/>

and may be reference by any other subsequent (using a local variable before it
is defined is not allowed) element in the document through
the use of the square brackets ``[]`` in the following manner::

    <some_element name="my_element" some_property="[my_variable]"/>

Global variables which persist across any subsequently included files may be specified through::

    <global_define name="my_global_variable" value="an_important_value"/>

Both local and global variables my be undefined (removed from the parser's scope)
in the following manner::

    <undefine name="my_variable"/>
    <global_undefine name="my_global_variable"/>

Occasionally the user may wish to specify a variable which can be modified from the command
line as an argument passed to *Kassiopeia*. To do this for a variable called ``my_random_seed``
the syntax is::

    <external_define name="my_random_seed" value="123"/>

This is useful for running large batches of similar simulations. For example, to simulate many
independent tracks the user might want to run the same simulation repeatedly, but use a different
random seed when starting the simulation. The value of ``my_random_seed`` can be changed
from its default value of 123 from the command line call to *Kassiopeia* in the following manner::

    Kassiopeia ./my_simulation.xml -r my_random_seed=456


Including external files
~~~~~~~~~~~~~~~~~~~~~~~~

Including external XML files is also supported through a relatively simple syntax.
This is helpful when a simulation is too complex to be managed by a single file.
A separate XML file can be included using the following::

    <include name="/path/to/file/my_file.xml"/>

This include expression may also be readily reconfigured through the use of a variable,
for example::

    <external_define name="my_file_name" value="my_file.xml"/>
    <include name="/path/to/file/[my_file_name]"/>

could be modified to include an entirely different file by passing another file name
argument to *Kassiopeia* using:

    Kassiopeia ./my_simulation.xml -r my_file_name=my_other_file.xml

This feature is particularly useful and enables the user to swap in an
entirely different configuration for some portion of the simulation
by passing a single command line variable.

Expressions
~~~~~~~~~~~

The ability to calculate in-line formulas is another useful feature. However, is only available when
*Kassiopeia* is linked against ROOT. Currently, the underlying implementation of the formula
processor relys on ROOT's TFormula_ class, and as such can process any expression which is valid
syntax for a TFormula. In order to active the formula mode, the relevant expression must
be enclosed in curly braces ``{}``. Variables may also be used within a formula.
An example of the formula syntax is given in
the following variable definition::

<define name="my_variable" value="4.0"/>
<define name="length" value="{2.3 + 2.0/sqrt([my_variable])}"/>

This example results in the variable ``[length]`` taking the value of 3.3.

Conditional Expressions and Looping
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In addition to mathematical expressions simple boolean conditions can be specified.
These are often helpful for swapping or toggling on/off different features. An
example showing the inclusion/exclusion of a brief section of XML is shown below::

    <define name="var1" value="1"/>
    <define name="var2" value="0"/>
    <if condition="{[var1] eq [var2]">
        <!-- intervening section of xml to be included/excluded -->
    </if>

Another feature which is indispensable when assembling complicated or repetitive geometries
is the the ability to insert multiple copies of an XML fragment with slight modifications.
This is called looping and is somewhat similar to the way a ``for`` loop functions in C++. It is
however, a purely static construct intended only to reduce the amount text needed to describe a
geometry or other feature. An example of its use can be found in the ``DipoleTrapMeshedSpaceSimulation.xml``
example. The example of the loop syntax showing the placement of several copies of a surface
with the name ``intermediate_z_surface`` is given below::

    <loop variable="i" start="0" end="10" step="1">
        <surface name="intermediate_z[i]" node="intermediate_z_surface">
            <transformation displacement="0. 0. {-0.5 + [i]*(0.4/10.)}"/>
        </surface>
    </loop>

Loops may also be nested when needed.

Messaging
---------

*Kassiopeia* provides a very granular means of reporting and logging simulation details of interest.
This feature is particularly useful when modifying the code and debugging specific features.
For example, in the file ``QuadrupoleTrapSimulation.xml`` begins with a section describing the
verbosity of each simulation element and the location of the logging file (described by
the variable ``log_path`` and the ``file`` element)::

    <define name="log_path" value="[KASPERSYS]/log/Kassiopeia"/>

    <messages>

        <file path="[log_path]" base="QuadrupoleTrapLog.txt"/>

        <message key="k_file" terminal="normal" log="warning"/>
    	<message key="k_initialization" terminal="normal" log="warning"/>

        <message key="kg_core" terminal="normal" log="warning"/>
    	<message key="kg_shape" terminal="normal" log="warning"/>
        <message key="kg_mesh" terminal="normal" log="warning"/>
        <message key="kg_axial_mesh" terminal="normal" log="warning"/>

    	<message key="ks_object" terminal="debug" log="normal"/>
        <message key="ks_operator" terminal="debug" log="normal"/>
    	<message key="ks_field" terminal="debug" log="normal"/>
    	<message key="ks_geometry" terminal="debug" log="normal"/>
        <message key="ks_generator" terminal="debug" log="normal"/>
    	<message key="ks_trajectory" terminal="debug" log="normal"/>
    	<message key="ks_interaction" terminal="debug" log="normal"/>
        <message key="ks_navigator" terminal="debug" log="normal"/>
    	<message key="ks_terminator" terminal="debug" log="normal"/>
    	<message key="ks_writer" terminal="debug" log="normal"/>
    	<message key="ks_main" terminal="debug" log="normal"/>
    	<message key="ks_run" terminal="debug" log="normal"/>
    	<message key="ks_event" terminal="debug" log="normal"/>
    	<message key="ks_track" terminal="debug" log="normal"/>
    	<message key="ks_step" terminal="debug" log="normal"/>

    </messages>

For the verbosity settings, you can independently set the verbosity that you see
in the terminal and the verbosity that is put into log files.
Furthermore, you can do that for each different part of Kassiopeia.
That way, for example, if you want a lot of detail on what's happening in
the navigation routines, you can increase the verbosity from only that part
of Kassiopeia, without being flooded with messages from everything else.
There are three possible verbosity levels, they are ``debug``, ``normal``, and ``warning``.
Of the three, ``warning`` is the least verbose, only reporting on non-fatal errors that
may be encountered during tracking. The ``normal`` mode will include a relatively small
set of details in addition to any warnings, while ``debug`` will provide an extremely extensive
description of the state of the simulation as it progresses.

Since there is so much information provided by the ``debug`` setting it substantially
slows down the speed of the simulation. In fact, in order to avoid unnecessarily slowing
down *Kassiopeia*, this option is completely disabled unless it is explicitly compiled into
the code by enabling the cmake option ``Kassiopeia_ENABLE_DEBUG`` during configuration.

Geometry
--------

The geometry section of the configuration file the first piece needed in order
to assemble a simulation. At its first and most basic level it is responsible for
defining all the different shapes that will be used, and placing them with respect
to one another in order to construct the arrangement that is needed.
For a full a description of all of the shape objects (surfaces and spaces)
which maybe constructed in *KGeoBag* see :ref:`basic-kgeobag-label` and :ref:`complex-kgeobag-label`.
The abstract base classes which serve as the interface between *KGeoBag* and *Kassiopiea*
are :kassiopeia:`KSSurface`, :kassiopeia:`KSSpace`, and :kassiopeia:`KSSide`.

The second half of the geometry section is responsible for adding
"extended" information to the previously described geometry elements.
These extensions can be properties such as colors for visualization
or boundary conditions and meshing details for the electromagnetic simulations.

Every relevant to the geometry description is processed by KGeoBag and must appear
between the start and end brackets::

    <geometry>
    <!-- fill in geometry description here -->
    </geometry>

It should be noted, that the full description of the geometry need not lie within the same
pair of ``<geometry>`` and ``</geometry>`` brackets. This facilitates the description of
separate geometry pieces in different files which may then be included and used in the final
assembly.

Shapes and Placement
~~~~~~~~~~~~~~~~~~~~

Of the example files provided, the ``DipoleTrapSimulation.xml`` has the simplest geometry. It
starts off with a description of each shapes involved::

    <!-- world -->

    <cylinder_space name="world_space" z1="-2." z2="2." r="2."/>

    <!-- solenoid -->

    <tag name="magnet_tag">
        <cylinder_tube_space
            name="solenoid_space"
            z1="-1.e-2"
            z2="1.e-2"
            r1="0.5e-2"
            r2="1.5e-2"
            radial_mesh_count="30"
        />
    </tag>

    <!-- ring -->

    <tag name="electrode_tag">
        <cylinder_surface
            name="ring_surface"
            z1="-2.0e-2"
            z2="2.0e-2"
            r="2.5e-1"
            longitudinal_mesh_count="200"
            longitudinal_mesh_power="3."
            axial_mesh_count="128"
        />
    </tag>

    <!-- tube -->

    <tag name="electrode_tag">
        <cylinder_surface
            name="tube_surface"
            z1="-1.e-2"
            z2="1.e-2"
            r="0.5e-2"
            longitudinal_mesh_count="200"
            longitudinal_mesh_power="3."
            axial_mesh_count="128"
        />
    </tag>

    <!-- target -->

    <tag name="target_tag">
        <disk_surface name="target_surface" r="1.0e-2" z="0."/>
    </tag>

    <!-- center -->

    <tag name="center_tag">
        <disk_surface name="center_surface" r="2.5e-1" z="0."/>
    </tag>

These shapes are then placed into an assembly of the experiment
geometry. Geometric objects are placed by referencing each shape by its given (unique!!) name
and specifying a transformation (relative the assembly origin) defining the location
and orientation of each object. The transformation types are displacements (specified by
a vector), and rotations (specified by an axis-angle pair or a
series of Euler angles using the Z-Y'-Z'' convention)::

    <space name="dipole_trap_assembly">
        <surface name="ring" node="ring_surface"/>
        <surface name="center" node="center_surface"/>
        <space name="downstream_solenoid" node="solenoid_space">
            <transformation displacement="0. 0. -0.5"/>
        </space>
        <surface name="downstream_tube" node="tube_surface">
            <transformation displacement="0. 0. -0.5"/>
        </surface>
        <surface name="upstream_target" node="target_surface">
            <transformation displacement="0. 0. -0.48"/>
        </surface>
        <space name="upstream_solenoid" node="solenoid_space">
            <transformation displacement="0. 0. 0.5"/>
        </space>
        <surface name="upstream_tube" node="tube_surface">
            <transformation displacement="0. 0. 0.5"/>
        </surface>
        <surface name="downstream_target" node="target_surface">
            <transformation displacement="0. 0. 0.48"/>
        </surface>
    </space>

Finally, the full assembly is placed within the world volume::

    <space name="world" node="world_space">
        <space name="dipole_trap" tree="dipole_trap_assembly"/>
    </space>

It should be noted that transformations applied to an assembly are collectively applied
to all of the geometric elements within the assembly. For example, placing
the dipole trap assembly within the world volume as::

    <space name="world" node="world_space">
        <space name="dipole_trap" tree="dipole_trap_assembly"
            <transformation rotation_euler="90. 0. 0." displacement="0 0 1.0"/>
        </space>
    </space>

which would rotate the whole assembly by 90 degrees about the z-axis, and then displace it by 1 meter
along the z-axis. Assemblies may be nested within each other and the coordinate transformations
which are associated with the placement each assembly will be appropriately
applied to all of the elements they contain.

Extensions
~~~~~~~~~~

In order to give physical properties to the geometry elements that have been constructed
and placed they must be associated with extensions. The currently available extensions are
axial (rotationally symmetric) or non-symmetric meshing, visualization properties, electrostatic
boundary conditions (Dirichlet or Neuman surfaces), and electromagnet properties such as current density.

A simple extension example is specifying the color and opacity
of a shape for its display in a VTK visualization window is as follows::

    <appearance name="app_magnet" color="0 255 127 127" arc="72" surfaces="world/dipole_trap/@magnet_tag"/>

This example tells the visualization that any shape given the tag ``magnet_tag`` should be colored
with an RGBA color value of (0,255,127,127). If you have VTK enabled you may wish to experiment
with the changes introduced by modifying these parameters.

Extensions may be attached to a shape object, either by specifying its name or location within
the geometry tree, or by giving a tag that is associated with it. The location of objects within
the geometry tree uses an XPath like syntax augmented with tagging. The tagging feature is very useful
for applying properties to many different elements at once. To do this, each element which is to
receive the same extension must share the same tag. There is no limit to the number of tags an geometric
element may be given, and tags may be nested. For example, given the dipole trap geometry as specified, one
may associate an axially symmetric mesh with all elements that share the tag ``electrode_tag`` with::

    <axial_mesh name="mesh_electrode" surfaces="world/dipole_trap/@electrode_tag"/>

The ``@`` symbol signifies that any geometric shape with the tag ``electrode_tag``
that is found within ``world/dipole`` trap should be giving an axial mesh extension (i.e. it will
be divided into a collection of axially symmetric objects, cones, cylinders, etc.). The axial mesh
will be later used by the field solving routines. However, a tag is not strictly necessary to
apply an extension, for example, if we wished to generate an axial mesh for everything within the world
we would write::

    <axial_mesh name="mesh_electrode" surfaces="world/@"/>

or, if we wished to single out the ``ring_surface`` shape exactly we would write::

     <axial_mesh name="mesh_electrode" surfaces="world/dipole_trap/ring"/>

Meshing is critical for any problem with involves electrostatic fields. The type of mesh depends on
the symmetry of the geometry. For completely axially-symmetric geometries, the ``axial_mesh`` is recommended
so that the zonal harmonics field computation method may be used. For completely non-symmetric (3D) geometries
the mesh type would be specified as follows::

    <mesh name="mesh_electrode" surfaces="world/dipole_trap/@electrode_tag"/>

Because of the very shape-specific nature of the deterministic meshing which is provided by KGeoBag,
parameters (``mesh_count`` and ``mesh_power``) describing how the mesh is to be constructed
are given when specifying the shapes themselves. That being said, the mesh associated with a
specific shape will not be constructed unless the extension statement is present.

Another important extension for field solving is the specification of boundary conditions. For example
when solving the Laplace boundary value problem, one may specify that a particular surface exhibit
Dirichlet boundary conditions with a particular voltage through the use of the following extension::

    <electrostatic_dirichlet name="electrode_ring" surfaces="world/dipole_trap/ring" value="-10."/>

Where ``value="-10"`` signifies that this surface has a potential of -10 volts.

For further demonstrations of the possible geometry extensions please see the provided example XML files.

Kassiopeia
----------

The remaining elements all live within the *Kassiopeia* element and must be placed with
start and end tags of the form::

    <kassiopeia>
    <!-- complete description of the kassiopeia simulation element here -->
    </kassiopeia>

The elements which must be described within the *Kassiopeia* namespace include the specification
of the fields, propagation, interactions, termination, navigation, output, and simulation.


Fields
------

Once the simulation geometry has been specified the user may describe the types of electric and
magnetic fields they wish associate with each geometric object. The field package *KEMField*
takes care of solving the boundary value problem and computing the fields for electrostatic problems.
It also handles the magnetic field computation from static current distributions. Fast field calculation
methods are available for axially symmetric (zonal harmonics) and
three dimensional problems (fast multipole method). The abstract base classes responsible for
electric and magnetic fields in *Kassiopeia* are :kassiopeia:`KSElectricField` and
:kassiopeia:`KSMagneticField` respectively.

For example, in the ``DipoleTrapSimulation.xml`` example the electric and magnetic fields are axially
symmetric and can be computed using the zonal harmonics expansion.

Electric
~~~~~~~~

To specify the electric field, the geometric surfaces which are electrically
active must be listed in the ``surfaces`` element. It is important that the surfaces which are specified have a mesh extension
and a boundary type extension. If either of these extensions are missing from the specified
surface, they will not be included in the electrostatics problem. A boundary element
mesh is needed to solve the Laplace equation using the boundary element method. Each element
of the mesh inherits its parent surface's boundary condition type.

Both a method to solve the Laplace boundary value problem (a ``bem_solver``), and a method by which to compute
the fields from the resulting charge densities must be given (a ``field_sovler``). In
the following example we use a ``robin_hood_bem_solver`` and a ``zonal_harmonic_field_solver``::

    <ksfield_electrostatic
        name="field_electrostatic"
        directory="[KEMFIELD_CACHE]"
        file="DipoleTrapElectrodes.kbd"
        system="world/dipole_trap"
        surfaces="world/dipole_trap/@electrode_tag"
        symmetry="axial"
    >
        <robin_hood_bem_solver
            tolerance="1.e-10"
            check_sub_interval="100"
            display_interval="1"
            cache_matrix_elements="true"
        />
        <zonal_harmonic_field_solver
            number_of_bifurcations="-1"
            convergence_ratio=".99"
            convergence_parameter="1.e-15"
            proximity_to_sourcepoint="1.e-12"
            number_of_central_coefficients="500"
            use_fractional_central_sourcepoint_spacing="false"
            central_sourcepoint_spacing="1.e-3"
            central_sourcepoint_start="-5.2e-1"
            central_sourcepoint_end="5.2e-1"
            number_of_remote_coefficients="200"
            remote_sourcepoint_start="-5.e-2"
            remote_sourcepoint_end="5.e-2"
        />

It is also important that geometric elements be meshed appropriately with respect to symmetry.
In the case that the user wishes to use zonal harmonic field calculation routines,
an ``axial_mesh`` must be used.
If a normal (3D) mesh is used, zonal harmonics cannot function. Different mesh/symmetry
types cannot be combined within the same electric field solving element.

In the three-dimensional mesh case, either an integrating field solver, or a fast multipole field solver
may be used. The integrating field solver may be specified through inclusion of the
element::

    <integrating_field_solver/>

within the the ``ksfield_electrostatic`` element. As the integrating field solver
is quite simple, it does not require additional parameters.
The fast multipole field solver on the other hand is
somewhat more complex and requires a relatively large set of additional parameters to be specified in
order to configure its use according to the user's desired level of accuracy and computational effort.

For a complete list and description of the XML bindings available for the electric field solving routines,
navigate to the directory ``$KASPERSYS/config/KEMField/Complete``. The file ``ElectricFields.xml``
has examples of the binding for initializing electric field problems.

Magnetic
~~~~~~~~

The specification of the magnetic field solving routines is considerably simpler since there is no
need to solve a boundary value problem before hand. There are essentially two choices for solving magnetic
fields from static current distributions. These are; the zonal harmonics method for use with axially
symmetric current sources, and the integrating magnetic field solver which can be used on geometries
with more arbitrary distributions of current. Unlike electric fields, magnetic fields can
contain components with both axially symmetric and non-axially symmetric elements
within the same region with no adverse effects.

A complete list and set of examples of the XML bindings for magnetic fields can be found
in the file ``$KASPERSYS/config/KEMField/Complete/MagneticFields.xml``.

Further documentation on the exact methods and parameters
used in *KEMField* can be found in [2] and [3].

Generation
----------

The intial state of particle's to be tracked is set up using the generator mechanism. The
abstract base class of all particle generators is :kassiopeia:`KSGenerator`.
When generating a particle, there are five important initial parameters:

- PID: What is the particle type? For particle ID values, see the PDG_ numbering scheme.
- Energy: What is the initial energy of the particle?
- Position: What is the initial position of the particle?
- Direction: In what direction is the particle traveling?
- Time: How is the production of particles distributed in time during the simulation?

Each of the dynamic components (energy, position, direction, time) can be draw
from a selected probability distribution. In some scenarios a dedicated particle generator
may be need which produces with a very specific and well defined particle state as the
result of some physical process (e.g. electron shake off in Radon decay). However,
as is often the case, the user may wish to modify each dynamic component in a specific
way in order to see what effect this has on the rest of the simulation. To
draw each dynamic component from an independent distribution a composite
generator is used. This type of generator combines a set of user selected distributions
to produce the initial energy, position, direction, and time parameters.
The following composite generator example is taken from the DipoleTrapSimulation.xml::

    <!-- pid=11 implies that electrons will be generated -->
    <ksgen_generator_composite name="generator_uniform" pid="11">
        <energy_composite>
            <energy_fix value="1."/>
        </energy_composite>
        <position_cylindrical_composite surface="world/dipole_trap/center">
            <r_cylindrical radius_min="0." radius_max="2.0e-1"/>
            <phi_uniform value_min="0." value_max="360."/>
            <z_fix value="0."/>
        </position_cylindrical_composite>
        <direction_spherical_composite surface="world/dipole_trap/center">
            <theta_fix value="0."/>
            <phi_uniform value_min="0." value_max="360"/>
        </direction_spherical_composite>
        <time_composite>
            <time_fix value="0."/>
        </time_composite>
    </ksgen_generator_composite>

In this example of the composite generator, the initial kinetic
energy of the particle is fixed to 1 eV and its position is drawn uniformly within a cylindrical
volume. Its initial starting time is fixed to zero, while its initial momentum direction is fixed
along the z-axis. All of the fixed values used in this composite generator may be replaced by
probability distributions. The available probability distributions depend on the quantity they are
intended to generator, but included uniform, gaussian, pareto, cosine, etc. Also available
is the ability to generate values at fix intervals throughout a limited range. For example
this can be done for energy as follows::

        <energy_composite>
            <energy_set name="e_set" value_start="1" value_stop="10" value_count="3"/>
        </energy_composite>

which would generate 3 electrons with energies equally spaced between 1 and 10 eV.
Alternatively, as specific list of values can also be used::

        <energy_composite>
            <energy_list
               add_value="11.8"
               add_value="20.5"
               add_value="33.1"
            />
        </energy_composite>

Keep in mind that if a ``list`` of ``set`` is used within a composite generator, the number of particles
produced in an "event" will be equal to multiplicative combination of all possible particle states.
For example, the following generator specification::

	<ksgen_generator_composite name="generator_uniform" pid="11">
        <energy_composite>
            <energy_set name="e_set" value_start="1" value_stop="200" value_count="10"/>
        </energy_composite>
        <position_cylindrical_composite surface="world/dipole_trap/center">
            <r_cylindrical radius_min="0." radius_max="2.0e-1"/>
            <phi_uniform value_min="0." value_max="360."/>
            <z_fix value="0."/>
        </position_cylindrical_composite>
        <direction_spherical_composite surface="world/dipole_trap/center">
            <theta_set name="e_set" value_start="0" value_stop="90" values_count="10"/>
            <phi_uniform value_min="0." value_max="360"/>
        </direction_spherical_composite>
        <time_composite>
            <time_fix value="0."/>
        </time_composite>
    </ksgen_generator_composite>

results in a total of 100 particles being generated in a single event (as a combination of
possible energies and momentum direction theta coordinate). To see other generator examples please
see the included example XML files.

Termination
-----------

The converse to particle generation is termination. The abstract base class of
all particle terminators is :kassiopeia:`KSTerminator`. Terminators are used to kill particle tracks
in situations where further simulation of the particle is of no further interest. Terminators
typically operate on very simple conditional logic. For example, a particle track may be terminated
if the particle's kinetic energy drops below some set value, or if it intersects a particular surface.

An example of a terminator which kills particle tracks which exceed a certain number (1000)
of allowed steps is given as follows::

    <ksterm_max_steps name="term_max_steps" steps="1000"/>

A pair of terminators which will kill a particle that exceeds an allowed range
for the z-coordinate is given in the following example::

    <ksterm_max_z name="term_max_z" z="1.0"/>
    <ksterm_min_z name="term_min_z" z="-1.0"/>

There are a wide variety of terminators currently avaiable, and the user is encourages to peruse
the XML example files as well as the source code to determine what (if any)
type of pre-existing terminator might be useful for their purpose.

Interactions
------------

In between generation and termination discrete stochastic interactions involving
the particle of interest may be applied during tracking. These interactions are divided
according to whether they are active in a volume, or on a surface.

Volume Interactions
~~~~~~~~~~~~~~~~~~~

Volume interactions typically involve scattering off of a gas. The
abstract base class of all volume interactions is :kassiopeia:`KSSpaceInteraction`.
Simple situations where the scattering interactions is treated
approximately by a constant density and cross section can be
constructed as follows::

    <ksint_scattering name="int_scattering" split="true">
        <density_constant temperature="300." pressure="3.e0"/>
        <calculator_constant cross_section="1.e-18"/>
    </ksint_scattering>

In this example the density is computed according to the ideal gas law from the temperature (Kelvin)
and pressure (Pascal) and the cross section is treated as a constant (independent of particle energy).
For more complicated interaction (e.g. involving differential or energy dependent cross sections) the user
may need to devise their own interaction class. Volume interactions must be associated with a particular
volume when describing the simulation structure.

Surface Interactions
~~~~~~~~~~~~~~~~~~~~

Surface interactions are much more limited in their scope and only
occur when the track of a particle passes through a surface. The
abstract base class of all surface interactions is :kassiopeia:`KSSurfaceInteraction`.

For example, to cause a particle to be reflected diffusely (Lambertian)
from a surface the user can specify an interaction of the following type::

    <ksint_surface_diffuse name="int_surface_diffuse" probability=".3" reflection_loss="0." transmission_loss="1."/>

In order for this interaction to operate on any particles it must be associated with a surface
in the simulation command structure.

Output
------

The data which is saved as output from the simulation requires two pieces: a file writer
and a description of the data to be saved. The abstract base class of all
file writers is :kassiopeia:`KSWriter`.

Writers
~~~~~~~

The file writer is responsible for buffering and writing the desired information to disk. The default writer
is based on ROOT_, and stores the output in a TTree::

    <kswrite_root
        name="write_root"
        path="/path/to/desired/output/directory"
        base="my_filename.root"
    />

If *Kassiopeia* is linked against VTK, an additional writer will be made available which can save
track and step information to a .vtp (VTK polydata file). This data is useful for visualalization
in external tools such as Paraview_. This write may be created using the following statement::

    <kswrite_vtk
        name="write_vtk"
        path="/path/to/desired/output/directory"
        base="my_filename_base.vtp"
    />

Output description
~~~~~~~~~~~~~~~~~~

The user may tailor the data written to disk to keep precisely the quantities of interest
and no more. To do this a description of the data components to be kept at the track
and step level must be given. An example of this (taken from the QuadrupoleTrapSimulation.xml example)
is shown below::


    <ks_component_member name="component_step_final_particle" field="final_particle" parent="step"/>
    <ks_component_member name="component_step_position" field="position" parent="component_step_final_particle"/>
    <ks_component_member name="component_step_length" field="length" parent="component_step_final_particle"/>

    <ks_component_group name="component_step_world">
        <component_member name="step_id" field="step_id" parent="step"/>
        <component_member name="continuous_time" field="continuous_time" parent="step"/>
        <component_member name="continuous_length" field="continuous_length" parent="step"/>
        <component_member name="time" field="time" parent="component_step_final_particle"/>
        <component_member name="position" field="position" parent="component_step_final_particle"/>
        <component_member name="momentum" field="momentum" parent="component_step_final_particle"/>
        <component_member name="magnetic_field" field="magnetic_field" parent="component_step_final_particle"/>
        <component_member name="electric_field" field="electric_field" parent="component_step_final_particle"/>
        <component_member name="electric_potential" field="electric_potential" parent="component_step_final_particle"/>
        <component_member name="kinetic_energy" field="kinetic_energy_ev" parent="component_step_final_particle"/>
    </ks_component_group>

    <ks_component_group name="component_step_cell">
        <component_member name="polar_angle_to_z" field="polar_angle_to_z" parent="component_step_final_particle"/>
        <component_member name="polar_angle_to_b" field="polar_angle_to_b" parent="component_step_final_particle"/>
        <component_member name="guiding_center_position" field="guiding_center_position" parent="component_step_final_particle"/>
        <component_member name="orbital_magnetic_moment" field="orbital_magnetic_moment" parent="component_step_final_particle"/>
    </ks_component_group>

    <ks_component_member name="component_track_initial_particle" field="initial_particle" parent="track"/>
    <ks_component_member name="component_track_final_particle" field="final_particle" parent="track"/>
    <ks_component_member name="component_track_position" field="position" parent="component_track_final_particle"/>
    <ks_component_member name="component_track_length" field="length" parent="component_track_final_particle"/>

    <ks_component_member name="z_length" field="continuous_length" parent="step"/>
    <ks_component_group name="component_track_world">
        <component_member name="creator_name" field="creator_name" parent="track"/>
        <component_member name="terminator_name" field="terminator_name" parent="track"/>
        <component_member name="total_steps" field="total_steps" parent="track"/>
        <component_member name="initial_time" field="time" parent="component_track_initial_particle"/>
        <component_member name="initial_position" field="position" parent="component_track_initial_particle"/>
        <component_member name="initial_momentum" field="momentum" parent="component_track_initial_particle"/>
        <component_member name="initial_magnetic_field" field="magnetic_field" parent="component_track_initial_particle"/>
        <component_member name="initial_electric_field" field="electric_field" parent="component_track_initial_particle"/>
        <component_member name="initial_electric_potential" field="electric_potential" parent="component_track_initial_particle"/>
        <component_member name="initial_kinetic_energy" field="kinetic_energy_ev" parent="component_track_initial_particle"/>
        <component_member name="initial_polar_angle_to_z" field="polar_angle_to_z" parent="component_track_initial_particle"/>
        <component_member name="initial_azimuthal_angle_to_x" field="azimuthal_angle_to_x" parent="component_track_initial_particle"/>
        <component_member name="initial_polar_angle_to_b" field="polar_angle_to_b" parent="component_track_initial_particle"/>
        <component_member name="initial_orbital_magnetic_moment" field="orbital_magnetic_moment" parent="component_track_initial_particle"/>
        <component_member name="final_time" field="time" parent="component_track_final_particle"/>
        <component_member name="final_position" field="position" parent="component_track_final_particle"/>
        <component_member name="final_momentum" field="momentum" parent="component_track_final_particle"/>
        <component_member name="final_magnetic_field" field="magnetic_field" parent="component_track_final_particle"/>
        <component_member name="final_electric_field" field="electric_field" parent="component_track_final_particle"/>
        <component_member name="final_electric_potential" field="electric_potential" parent="component_track_final_particle"/>
        <component_member name="final_kinetic_energy" field="kinetic_energy_ev" parent="component_track_final_particle"/>
        <component_member name="final_polar_angle_to_z" field="polar_angle_to_z" parent="component_track_final_particle"/>
        <component_member name="final_azimuthal_angle_to_x" field="azimuthal_angle_to_x" parent="component_track_final_particle"/>
        <component_member name="final_polar_angle_to_b" field="polar_angle_to_b" parent="component_track_final_particle"/>
        <component_member name="final_orbital_magnetic_moment" field="orbital_magnetic_moment" parent="component_track_final_particle"/>
        <component_member name="z_length_internal" field="continuous_length" parent="track"/>
        <component_integral name="z_length_integral" parent="z_length"/>
    </ks_component_group>


Propagation
-----------

The propagation section is used to describe the physical process which is associated
with the movement of a particle, and also the means by which the equations of motion are solved.
The equations of motions are solved numerically with various control methods for the time step
used during integration of a specific particle trajectory. The
abstract base class of all particle trajectories is :kassiopeia:`KSTrajectory`.

There are essential four different equations of motion (trajectories) available for particle tracking in *Kassiopeia*.
These are, exact, adiabatic, electric, and magnetic.

The first trajectory type is the so-called exact method, which solves the Lorentz equation for charged
particles exactly without approximations. The second method is the adiabatic method, which is useful
for solving charged particle motion in the presence of smoothly varying magnetic fields. The last two
types, electric and magnetic, are used for tracing the field lines of static electric and
magnetic fields respectively.

The exact tracking method can be used where accuracy is of the utmost importance, but requires
a large number of steps in order propagate a particle for a long time or distance. An example
of its use is given below::

    <kstraj_trajectory_exact name="trajectory_exact" attempt_limit="8">
        <interpolator_crk name="interpolator_crk"/>
        <integrator_rkdp853 name="integrator_rkdp853"/>
        <term_propagation name="term_propagation"/>
        <control_position_error name="control_position_error" absolute_position_error="1e-12" safety_factor="0.75" solver_order="8"/>
        <control_length name="stepsizelength" length="1e-4" />
        <control_time name="stepsizetime" time="1e-6" />
    </kstraj_trajectory_exact>

In the above example, the integrator type specified ``integrator_rkdp853`` is an 8-th order Runge-Kutta
integrator with a 7-th order dense output interpolant (specified by ``interpolator_crk``). The step-size
control is accomplished through the combination of three different methods. Two of them ``control_length``
and ``control_time`` place a fixed limit on the step-size, while the third ``control_position_error`` dynamically
attempts to limit the local numerical error on the particle's position below some limit.

All of the trajectories take an optional parameter ``attempt_limit`` which limits the
number of tries an integration step will be re-attempted if a step-size control determines
that the step has failed. The default number of maximum number of attempts is 32. If a trajectory
makes no progress after the maximum number of allowed attempts, the track will be terminated
with a ``trajectory_fail`` flag.

An example of the adiabatic trajectory is specified in the following::

    <!--  adiabatic trajectory -->
    <kstraj_trajectory_adiabatic name="trajectory_adiabatic" piecewise_tolerance="1e-6" max_segments="130" use_true_position="true" cyclotron_fraction="{1.0/64.0}" attempt_limit="8">
        <interpolator_crk name="interpolator_crk"/>
        <integrator_rkdp853 name="integrator_rkdp853"/>
        <term_propagation name="term_propagation"/>
        <term_drift name="term_drift"/>
        <term_gyration name="term_gyration"/>
        <control_time name="control_time_adiab" time="1e-6"/>
        <control_position_error name="control_position_error" absolute_position_error="1e-9" safety_factor="0.75" solver_order="8"/>
    </kstraj_trajectory_adiabatic>

In this example, both the motion of the particle's guiding center drift (``term_drift``) and the
particle's local gyration (``term_gyration``) itself are included in the equations of motion and computed.
It is important to note that the adiabatic trajectory cannot be use if there is no magnetic field present.

The last two trajectory types can be specfified as follows::

    <!--  magnetic trajectory -->
    <kstraj_trajectory_magnetic name="trajectory_magnetic" piecewise_tolerance="1e-12" max_segments="128">
        <interpolator_crk name="interpolator_crk"/>
        <integrator_rkdp54 name="integrator_rkdp54"/>
        <term_propagation name="term_propagation" direction="forward"/>
        <control_time name="control_time" time="1.e-4"/>
        <control_length name="control_length" length="1e-2"/>
    </kstraj_trajectory_magnetic>

    <!--  electric trajectory -->
    <kstraj_trajectory_electric name="trajectory_electric" piecewise_tolerance="1e-12" max_segments="128">
        <interpolator_crk name="interpolator_crk"/>
        <integrator_rkdp54 name="integrator_rkdp54"/>
        <term_propagation name="term_propagation" direction="backward"/>
        <control_time name="control_time" time="1.e-4"/>
        <control_length name="control_length" length="1e-2"/>
    </kstraj_trajectory_electric>

The electric and magnetic field line trajectories are useful for visualization and during
the design stages of an experiment or simulation. Note that in the above examples we have used a faster but less
accurate Runge-Kutta integrator ``integrator_rkdp54``. The property ``direction`` in ``term_propagation``
can be used to specify the direction in which field lines are tracked (positive-to-negative or north-to-south).

In all of the trajectories (exact, adiabatic, electric, magnetic) the parameters ``piecewise_tolerance`` and ``max_segments`` are optional.
They dictate how a particles trajectory (in between steps) should be approximated by piecewise linear segments
when determining surface and volume intersections. These parameters are only used
when the ``ksnav_meshed_space`` navigator is in use, and determine how accurately intersections
are found. The default ``ksnav_space`` navigator ignores these parameters, since it
uses a faster but less accurate approximation by linearly interpolating a
particle's intermediate state and position.

The adiabatic trajectory also takes the additional parameters
``use_true_position`` and ``cyclotron_fraction`` when the ``mesh_spaced_navigator``
is in use. The parameter ``use_true_position`` determines whether the particle or its
guiding center position is used for the purpose of finding intersections. If the particle's
true position is used, then the parameter ``cyclotron_fraction`` dictates the number of linear
segments the semi-helical path is broken into.

Navigation
----------

The navigation of a particle is split into two components, surface navigation and space
navigation. Surface navigation is very simple and only takes place when a particle
has intersected an active surface. The surface navigator determines whether the state
of the particle is modified on the surface and whether it is reflected or transmitted.
It is very simple and can be made available for use with the declaration::

    <ksnav_surface name="nav_surface"/>

The space navigator more complex since it is responsible for determine the location of the particle
and any possible intersections it may have with real of virtual boundaries. It
is also responsible for ensuring that the current simulation state matches the configuration
specified by the user. The spaces that the navigator considers may be real objects (such as a vacuum chamber wall) or
virtual (which only modify the state of the simulation, e.g. exchanging a trajectory method).

For space navigation there are two options. The first is the default ``ksnav_space`` navigator
which can be specified as below::

    <ksnav_space name="nav_space" enter_split="false" exit_split="false"/>

As this navigator is very simple, it does not take many additional parameters.
The parameters ``enter_split`` and ``exit_split`` determine whether or not a track
is split in two (for the purposes of output/saving data) upon entrance or exit of a geometric space.

The second is the ``ksnav_meshed_space`` navigator which is intended to be used
in complex three-dimensional geometries where it has better performance over the default navigator.
An example of this is shown in the PhotoMultplierTube.xml file. Its configuration is somewhat
more complicated as it performs the navigations on the meshed boundaries of spaces and surfaces.
This navigation method requires the construction of an octree spatial partition (which is cached for later
re-use). The use must specify the depth of the octree (``max_octree_depth``) and the number of elements
allowed in a octree node before a subdivision is required (``n_allowed_elements``). In addition,
the root of the geometry tree must also be specified with the parameter ``root_space``,
this is typically the world volume::

    <ksnav_meshed_space name="nav_meshed_space" root_space="space_world" max_octree_depth="9" n_allowed_elements="1"/>

Though they are not shown (they default to false) the exit and entrance split
parameters may also be set for the ``ksnav_meshed_space`` navigator.
Because the ``ksnav_meshed_space`` navigator requires a boundary mesh in
order to operate, all geometric objects (spaces, sufaces)
which have navigation commands attached to them must also have a mesh extension in the geometry specification.
Furthermore, since ``ksnav_meshed_space`` requires access to the root space ``space_world``
and all of the navigation commands associated with the shapes it contains,
it must be declared after the definition of the simulation command structure element ``ksgeo_space``.


Navigation and Commands
-----------------------

The commands which are associated with particular surfaces and spaces are what governs
the state of a *Kassiopeia* simulation as a particle is tracked. They are declared
through the specification of a ``ksgeo_space``. A very simple example of the declaration
of the command structure can be seen in the DipoleTrapSimulation.xml as shown below::

    <ksgeo_space name="space_world" spaces="world">
        <command parent="root_terminator" field="add_terminator" child="term_max_steps"/>
        <command parent="root_terminator" field="remove_terminator" child="term_world"/>
        <command parent="write_root" field="add_track_output" child="component_track_world"/>
        <command parent="write_root" field="add_step_output" child="component_step_world"/>
        <geo_surface name="surface_upstream_target" surfaces="world/dipole_trap/upstream_target">
            <command parent="root_terminator" field="add_terminator" child="term_upstream_target"/>
        </geo_surface>
        <geo_surface name="surface_downstream_target" surfaces="world/dipole_trap/downstream_target">
            <command parent="root_terminator" field="add_terminator" child="term_downstream_target"/>
        </geo_surface>
        <geo_surface name="surface_center" surfaces="world/dipole_trap/center"/>
    </ksgeo_space>

In this example we create a ``ksgeo_space`` navigation space using the ``world`` space
geometric object. Inside of this world volume we declare a series of command which will be
executed any time a particle enters or is initialized within the world volume. The first two
commands add and remove specific terminators, while the next two declare what sort of output
should be written down within the world volume. Following that, there are commands which are
attached to specific surfaces which are present. For example, attaching the terminator ``term_upstream_target``
ensures that a particle impinging on the surface ``surface_upstream_target`` will be killed.
Commands can used to change the active field calculation method,
swap trajectory types, or add/remove interactions, etc. Various spaces and
their associated commands can be nested within each other allowing for a very
flexible and dynamic simulation configuration.

Simulation
----------

The final object to be declared within ``<kassiopeia>`` is the simulation element. This describes
the simulation object :kassiopeia:`KSSimulation`, which is then executed by :kassiopeia:`KSRoot`.
The simulation element specifies the global and initial properties of the simulation as a whole.
For example in the QuadrupoleTrapSimulation.xml example, the simulation element
is declared as follows::

    <ks_simulation
        name="quadrupole_trap_simulation"
        run="1"
        seed="51385"
        events="10"
        magnetic_field="field_electromagnet"
        electric_field="field_electrostatic"
        space="space_world"
        generator="generator_uniform"
        trajectory="trajectory_exact"
        space_navigator="nav_space"
        surface_navigator="nav_surface"
        writer="write_root"
    />

The ``run`` is simply a user provided identifier. The ``seed`` is the value provided to
the global (singleton) random number generator. Simulations with the same configuration and same seed
should provide identical results. If the user is interested in running *Kassiopeia* on many machines
in order to achieve high throughput particle tracking,
care must be taken to ensure that the ``seed`` value is different
for each run of the simulation. The parameter ``events`` determines the total number of times that
the generator is run (but this is not necessarily the number of particles that will be tracked).
The remaining parameters ``magnetic_field``, ``space``, ``generator``, etc. all specify the default
objects to be used for the initial state of the simulation (commands specified within ``ksgeo_space``)
may modify the actual objects used during the course of a simulation.

Following the declaration of ``ks_simulation``, the closing tag ``</kassiopeia>`` is placed
to complete the simulation configuration. When this tag is encountered by the XML parser, it triggers
the simulation to run.


Visualization
-------------

If *Kassiopeia* has been linked against, simple visualization of the simulation may be made available.
By adding the `vtk_window` tag we enable a VTK window that will open when the simulation is complete.
This element needs a ``vtk_geometry_painter`` and the ```vtk_track_painter`` to draw the geometry
and tracks, respectively. An example of this type of visualization from the
QuadurpoleTrapSimulation.xml file is a follows::

    <vtk_window
        name="vtk_window"
        enable_display="true"
        enable_write="true"
        frame_title="KGeoBag Visualization"
        frame_size_x="1024"
        frame_size_y="768"
        frame_color_red=".2"
        frame_color_green=".2"
        frame_color_blue=".2"
        view_angle="45"
        eye_angle="0.5"
        multi_samples="4"
        depth_peeling="10"
    >
        <vtk_geometry_painter
            name="geometry_painter"
            surfaces="world/quadrupole_trap/#"
        />
        <vtk_track_painter
            name="track_painter"
            path="[output_path]"
            file="QuadrupoleTrapSimulation.root"
            point_object="component_step_world"
            point_variable="position"
            color_object="component_step_cell"
            color_variable="polar_angle_to_b"
        />
    </vtk_window>

Note that the visualization window must be placed outside of the ``<kassiopeia>``...``</kassiopiea>``
environment tags.

Common Pitfalls and Problems
----------------------------

The XML parse does have some ability to recognize simple errors in a configuration file
and will generally report the location of an element which it is not able to process.

Some errors which might occur if a file is improperly configured are:

- Multiple objects which share the same name at the same scope.
- Misspelled element types.
- Missing closing brackets.
- Undefined variables.
- Undeclared (but used) elements.

In the case of more than one copy of the same object with the name <X>, the XML parser will fail with
with an error along the lines of::

[INITIALIZATION ERROR MESSAGE] Multiple instances of object with name <X>.

In the case where an element's type name is misspelled the parser will fail with
an unreconized element error. For example if we misspelled ``ksterm_max_z`` as ``kterm_max_z``
we would recieve the following error::

    [INITIALIZATION ERROR MESSAGE] nothing registered for element <kterm_max_z> in element <kassiopeia>

If there is a mis-matched bracket the intialization will usually fail with an unrecongnized
character error, such as::

[INITIALIZATION ERROR MESSAGE] element <X> encountered an error <got unknown character <<>>

If a variable "[X]" is used without being previously defined, and undefined error will be reported
as follows::

[INITIALIZATION ERROR MESSAGE] variable <X> is not defined

If there is an attempt to retrieve/reference and element which has not been declared the
the simulation will fail with the message::

[INITIALIZATION WARNING MESSAGE] No suitable Object called <X> in Toolbox


.. _TFormula: https://root.cern.ch/doc/master/classTFormula.html
.. _PDG: http://pdg.lbl.gov/mc_particle_id_contents.html
.. _Paraview: http://www.paraview.org
.. _ROOT: https://root.cern.ch/


.. rubric:: Footnotes

[1] Daniel Lawrence Furse. Techniques for direct neutrino mass measurement utilizing tritium [beta]-decay. PhD thesis, Massachusetts Institute of Technology, 2015.

[2] Thomas Corona. Methodology and application of high performance electrostatic field simulation in the KATRIN experiment. PhD thesis, University of North Carolina, Chapel Hill, 2014.

[3] John P. Barrett. A Spatially Resolved Study of the KATRIN Main Spectrometer Using a Novel Fast Multipole Method. PhD thesis, Massachusetts Institute of Technology, 2016.
