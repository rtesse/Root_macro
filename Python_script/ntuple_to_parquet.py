import sys
import os

from root_pandas import read_root


print(" Convert file " + sys.argv[1])
root_filename = sys.argv[1]

root_data = read_root(root_filename)
parquet_filename = os.path.splitext(root_filename)[0]+'.parquet.gzip'

root_data.to_parquet(parquet_filename, compression='gzip')
