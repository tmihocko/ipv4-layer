ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"

BUILD_DIR="$ROOT_DIR/build"
IMAGE_NAME="ipv4-stack"

case "$(uname -s)" in
	Linux)
		cmake \
			-S . \
			-B build \
			-DCMAKE_BUILD_TYPE=Debug \
	    
		cmake --build "$BUILD_DIR" --parallel
		;;
	Darwin) # MacOS
		docker build \
			--tag \
			"$IMAGE_NAME" \
			"$ROOT_DIR" 
		;;
	*) 
		echo "Unsupported operating system">&2
		exit 1
		;;
	
esac

# TODO:
# Check if docker info --format "{{.OSType}}" == "linux"
# Or else daemon doesnt run linux, error
# echo "Cannot run on this docker daemon">&2
