rm -R .\build
mkdir build | OUT-NULL
Push-Location .\build
try
{
    cmake -DUSE_SE_BUILD=ON ..
    cmake --build . --config Release -v
}
finally
{
    Pop-Location
}