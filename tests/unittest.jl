include("../Tmp/data/ExampleMC.jl")
include("../Tmp/data/ExampleWithVectorMember.jl")
include("../Tmp/data/ExampleForCyclicDependency1.jl")
include("../Tmp/data/ExampleForCyclicDependency2.jl")
include("../Tmp/data/ExampleWithOneRelation.jl")
include("../Tmp/data/ExampleCluster.jl")

using Test
@testset "Julia Bindings" begin
	@testset "Relations" begin

	    mcp1 = ExampleMC()
	    mcp1.PDG = 2212

	    mcp2 = ExampleMC()
	    mcp2.PDG = 2212

	    mcp3 = ExampleMC()
	    mcp3.PDG = 1
	    push!(mcp3.parents,mcp1)

	    mcp4 = ExampleMC()
	    mcp4.PDG = -2
	    push!(mcp4.parents,mcp2)

	    mcp5 = ExampleMC()
	    mcp5.PDG = -24
	    push!(mcp5.parents,mcp1)
	    push!(mcp5.parents,mcp2)


	    mcp1.PDG = 12
	    mcp2.PDG = 13

	    # passes if values are changed in parents

	    @test mcp3.parents[1].PDG == 12
	    @test mcp4.parents[1].PDG == 13
	    @test mcp5.parents[1].PDG == 12
	    @test mcp5.parents[2].PDG == 13
	end

	@testset "Vector Members" begin

		m1 = ExampleWithVectorMember()
		m1.count = Float32[1,2,3,4,5]
		m1.count[5] = 6

		@test m1.count[5] == 6
		@test m1.count[1] == 1
		@test m1.count[2] == 2
		@test m1.count[3] == 3
		@test m1.count[4] == 4
	end

	@testset "Cyclic Dependency" begin

		cd1 = ExampleForCyclicDependency1()
		cd2 =  ExampleForCyclicDependency2()
		cd1.ref = cd2
		cd2.ref = cd1

		@test cd1.ref === cd2
		@test cd2.ref === cd1
	end

	@testset "One To One Relations" begin

		c1 = ExampleCluster()
		c1.energy = Float64(5)

		c2 = ExampleWithOneRelation()
		c2.cluster = c1

		@test c2.cluster.energy == Float64(5)

	end
end;
